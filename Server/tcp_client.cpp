// 客户端即发送 "普通数据" 又同时发生 "带外数据"
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
    int ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        perror("connect error");
        exit(-1);
    }

    // 循环发送"普通数据"和"带外数据"
    int count = 0;
    while(1)
    {
        char buf2[1024] = "abc";
        ret = send(sockfd, buf2, 1024, 0);
        if(ret < 0)
        {
            perror("send error");
            exit(-1);
        }

        if(count == 10)
        {
            char buf[1024] = "hello world!";
            int ret = send(sockfd, buf, 1024, MSG_OOB);
            if(ret < 0)
            {
                perror("send error");
                exit(-1);
            }
        }
        count++;
    }

    return 0;
}

