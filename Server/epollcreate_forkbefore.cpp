// 体会epoll的惊群现象
// 模拟场景: 进程惊群现象
//     在父进程中进行socket, bind, listen之后，然后将该socket加入到epoll当中，然后fork出来多次子进程进行epoll_wait
//     等待事件，然后通过nc连接到服务器检测是否会发生epoll惊群现象，之后直接关闭socket，不在模拟读写等操作。
// 结论:
//      在fork之前创建epoll_create，即所有进程使用同一颗红黑树。在新的内核中不会引发惊群问题，大致思路与accept相似
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std; 

#define SIZE 1024
#define ADDR "192.168.248.129"
#define PORT 8888

class Epoll 
{
    private:
        int epollfd;
        epoll_event* event;
        int index;
    public:
        void Epoll_Init()
        {
            epollfd = epoll_create(SIZE);
            if(epollfd < 0)
            {
                perror("epoll_create error!");
                exit(-1);
            }
            event = (epoll_event*)malloc(sizeof(epoll_event) * SIZE);
            if(event == NULL)
            {
                perror("malloc error!");
                exit(-1);
            }
            index = SIZE;
        }

        void Epoll_Add(int sockfd, int sockevent = EPOLLIN)
        {
            struct epoll_event event;
            event.events = sockevent;
            event.data.fd = sockfd;

            int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event);
            if(ret < 0)
            {
                perror("epoll_ctl error!");
                exit(-1);
            }
        }

        void Epoll_Wait(vector<int>& socklist, int timeout = -1)     // 阻塞等待 -> 方便实验
        {
            int ret = epoll_wait(epollfd, event, index, timeout);
            if(ret < 0)
            {
                perror("epoll_wait error!");
                exit(-1);
            }
            else if(ret == 0)
            {
                cout << "wait timeout!" << endl;
            }

            cout << "事件就绪！" << endl;
            for(int i = 0; i < index; ++i)
            {
                if(event[i].events & EPOLLIN)
                {
                    socklist.push_back(event[i].data.fd);
                }
            }
        }

        ~Epoll()
        {
            free(event);
        }
};

void SetNonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket error!");
        exit(-1);
    }

    SetNonblock(sockfd);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ADDR, (void*)&addr.sin_addr);
    addr.sin_port = htons(PORT);
    int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        perror("bind error!");
        exit(-1);
    }

    listen(sockfd, 128);

    Epoll ep;
    ep.Epoll_Init();
    ep.Epoll_Add(sockfd);

    // epoll_create在fork之前调用，即全局使用一颗红黑树 
    for(int i = 0; i < 10; ++i)
    {
        pid_t pid;
        if((pid = fork()) == 0)  // 子进程
        {
            vector<int> v;
            cout << getpid() << "进程等待阻塞事件！" << endl;
            ep.Epoll_Wait(v);
            cout << getpid() << "进程被事件唤醒！" << endl;

            // 处理事件
            for(auto fd : v)
            {
                if(fd == sockfd)    // 监听socket
                {
                    struct sockaddr_in cliaddr;
                    socklen_t len = sizeof(sockaddr_in);
                    int ret = accept(fd, (struct sockaddr*)&cliaddr, &len);  
                    if(ret < 0) 
                    { 
                        perror("accept error!"); 
                        exit(-1); 
                    }
                    else 
                    {
                        cout << getpid() << "进程连接成功！" << endl;
                    }
                }
            }

            exit(0);
        }
    }

    // 回收子线程
    while(waitpid(-1, NULL, WNOHANG) > 0);
    close(sockfd);

    return 0;
}
