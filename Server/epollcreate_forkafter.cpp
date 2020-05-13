// 模拟epoll的惊群问题
// 模拟场景:
//     主进程中创建socket，bind，listen，然后创建子进程，在子进程中epoll_create，epoll_wait等相关操作，阻塞等待事件发生，
//     当事件触发时，观察是否会唤醒所有的进程去accept事件。
// 实验结果:
//     产生惊群问题，唤醒了所有的进程。
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
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

#define PROSIZE 10

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
                perror("epoll_create error");
                exit(-1);
            }
            event = (epoll_event*)malloc(sizeof(epoll_event) * SIZE);
            if(event == NULL)
            {
                perror("malloc error");
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
                perror("epoll_ctl error");
                exit(-1);
            }
        }

        void Epoll_Wait(vector<int>& socklist, int timeout = -1)     // 阻塞等待 -> 方便实验
        {
            int ret = epoll_wait(epollfd, event, index, timeout);
            if(ret < 0)
            {
                perror("epoll_wait error");
                exit(-1);
            }
            else if(ret == 0)
            {
                cout << "wait timeout" << endl;
            }

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
        perror("socket error");
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
        perror("bind error");
        exit(-1);
    }

    listen(sockfd, 128);

    for(int i = 0; i < PROSIZE; ++i)
    {
        pid_t pid;
        if((pid = fork()) == 0)
        {
            Epoll ep;
            vector<int> v;
            ep.Epoll_Init();
            ep.Epoll_Add(sockfd);
            ep.Epoll_Wait(v);
            
            cout << getpid() << "进程被唤醒！" << endl;
            for(auto fd : v)
            {
                if(fd == sockfd)
                {
                    sleep(2);                   // 扩大现象 -> 检测是否唤醒了所有的进程  实验结果: 唤醒所有进程

                    struct sockaddr_in addr;
                    socklen_t len = sizeof(addr);
                    int ret = accept(sockfd, (struct sockaddr*)&addr, &len);
                    if(ret < 0)
                    {
                        if(errno == EAGAIN)
                        {
                            cout << "事件已经被处理！" << endl;
                        }
                        perror("accept error");
                        exit(-1);
                    }

                    cout << getpid() << "进程连接成功！" << endl;
                }
                else 
                {
                    // 通信套接字
                }
            }
        }
    }

    while(waitpid(-1, NULL, WNOHANG) > 0);
    close(sockfd);

    return 0;
}
