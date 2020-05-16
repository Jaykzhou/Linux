// 服务器即处理 "普通数据" 同时又处理 "带外数据"  -->  epoll
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

#define IP "192.168.248.129"
#define PROT 8888
#define EPSIZE 1024
#define ACCEPTQUE 128

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socker error");
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PROT);
    inet_pton(AF_INET, IP, &addr.sin_addr);
    int ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        perror("bind error");
        exit(-1);
    }

    listen(sockfd, ACCEPTQUE);

    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int clisockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
    if(clisockfd < 0)
    {
        perror("accept error");
        exit(-1);
    }

    int epfd = epoll_create(EPSIZE);
    if(epfd < 0)
    {
        perror("epoll create error");
        exit(-1);
    }

    // 添加可读事件 / 异常事件(带外数据)
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = clisockfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, clisockfd, &event);
    if(ret < 0)
    {
        perror("epoll ctl error");
        exit(-1);
    }

    while(1)
    {
        struct epoll_event events[EPSIZE];
        int eventnum = epoll_wait(epfd, events, EPSIZE, -1);
        if(eventnum <= 0)
        {
            perror("epoll wait error");
            close(sockfd);
            exit(-1);
        }

        // 检测对应事件
        for(int i = 0; i < eventnum; ++i)
        {
            if(events[i].events & EPOLLPRI)
            {
                // 带外数据
                char buf[1024];
                int ret = recv(events[i].data.fd, buf, 1024, MSG_OOB);
                if(ret < 0)
                {
                    perror("recv error");
                    exit(-1);
                }

                cout << "带外数据: " << buf << endl;
            }

            if(events[i].events & EPOLLIN)
            {
                // 普通数据
                char buf[1024];
                int ret = recv(events[i].data.fd, buf, 1024, 0);
                if(ret < 0)
                {
                    perror("recv error");
                    exit(-1);
                }

                cout << "普通数据: " << buf << endl;
            }
        }

        sleep(1);
    }

    return 0;
}

