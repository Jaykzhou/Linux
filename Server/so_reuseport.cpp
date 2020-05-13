// 解决epoll惊群问题 
// 解决思路:
//     使用SO_REUSEPORT特性在内核层面上解决epoll惊群的现象
// 具体实现:
//     创建子进程，在每个子进程中socket，bind，listen等操作为不同的进程绑定相同的端口，然后epoll_create，epoll_wait等相关操作
//     在不同进程中监听相同的端口，当连接请求时，内核会根据对端的ip与端口生一个hashcode，根据hashcode去唤醒等待队列中相应的进
//     程，从而避免了惊群现象。
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

#define SIZE 1024
#define PROSIZE 10

#define ADDR "192.168.248.129"
#define PROT 8888
#define RECVQUE 128

// epoll监听器
class Epoll 
{
    private:
        int epfd;
        epoll_event* event;
        int size;
    public:
        void Epoll_Init()
        {
            epfd = epoll_create(SIZE);
            if(epfd < 0)
            {
                perror("epoll create error");
                exit(-1);
            }
            event = (epoll_event*)malloc(sizeof(epoll_event) * SIZE);
            if(!event)
            {
                perror("malloc error");
                exit(-1);
            }
            size = SIZE;
        }

        void Epoll_Add(int sockfd, int sockevent = EPOLLIN)
        {
            struct epoll_event e;
            e.events = sockevent;
            e.data.fd = sockfd;

            int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &e);
            if(ret < 0)
            {
                perror("epoll contrl error");
                exit(-1);
            }
        }

        void Epoll_Wait(vector<int>& socklist, int timeout = -1)   // -1: 阻塞等待
        {
            int ret = epoll_wait(epfd, event, size, timeout);
            if(ret < 0)
            {
                perror("epoll wait error");
                exit(-1);
            }
            // == 0 -> wait timeout 
            
            for(int i = 0; i < size; ++i)
            {
                if(event[i].events & EPOLLIN)   // 事件发生，根据mask掩码对应标志位置为 1
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

void SetNonBlock(int fd)
{
    int flag = fcntl(fd, F_GETFL); 
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL);
}

void SetSOREUSEPORT(int fd)
{
    int flag = 1;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
    if(ret < 0)
    {
        perror("setsockopt error");
        exit(-1);
    }
}

int main()
{
    for(int i = 0; i < PROSIZE; ++i)
    {
        pid_t pid = -1;
        if((pid = fork()) == 0)
        {
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if(sockfd < 0)
            {
                perror("socket error");
                exit(-1);
            }

            SetNonBlock(sockfd);                 // 套接字非阻塞
            SetSOREUSEPORT(sockfd);              // 端口复用

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PROT);
            inet_pton(AF_INET, ADDR, &addr.sin_addr);
            int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
            if(ret < 0)
            {
                perror("bind error");
                exit(-1);
            }

            listen(sockfd, RECVQUE);
            
            // epoll 监听
            Epoll ep;
            ep.Epoll_Init();
            ep.Epoll_Add(sockfd);

            while(1)                // 循环处理事务
            {
                vector<int> socklist;
                ep.Epoll_Wait(socklist);
                
                cout << getpid() << "进程被唤醒！" << endl;
                for(auto fd : socklist)
                {
                    if(fd == sockfd)
                    {
                        // 监听套接字
                        struct sockaddr_in sockaddr;
                        socklen_t len = sizeof(sockaddr);
                        int ret = accept(sockfd, (struct sockaddr*)&sockaddr, &len);
                        if(ret < 0)
                        {
                            if(errno == EAGAIN)
                            {
                                cout << "连接请求已经被处理！" << endl;
                            }
                        }
                        else 
                        {
                            cout << getpid() << "连接成功建立！" << endl;
                        }
                    }
                    else 
                    {
                        // 通信套接字
                    }
                }
            }
        }
    }

    while(waitpid(-1, NULL, WNOHANG) > 0);
    
    return 0;
}

