// 使用 SO_REUSEPORT 新特性解决epoll惊群问题
// 具体思路:
//     循环创建多个子进程，在每个子进程对套接字设置so_reuseprot选项，并且将子进程绑定在该套接字上，然后将该套接字加入到epoll中，
//     调用epoll_wait进行事件等待，当事件发生时，检测是否有惊群现象发生。
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
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
}

int main()
{
    Epoll ep;
    ep.Epoll_Init();

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

            SetSOREUSEPORT(sockfd);              // 端口复用
            SetNonBlock(sockfd);                 // 套接字非阻塞

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PROT);
            inet_pton(AF_INET, ADDR, &addr.sin_family);
            int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
            if(ret < 0)
            {
                perror("bind error");
                exit(-1);
            }

            listen(sockfd, RECVQUE);
            
            // epoll 监听
            ep.Epoll_Add(sockfd);
            vector<int> socklist;
            ep.Epoll_Wait(socklist);

        }
    }

    while(waitpid(-1, NULL, WNOHANG) > 0);
    
    return 0;
}

