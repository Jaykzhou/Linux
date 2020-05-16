// select实现同时处理"普通数据"和"带外数据"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
using namespace std;

#define IP "192.168.248.129"
#define PROT 8888
#define ACCEPTQUE 128

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket error");
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
    int clisock = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
    if(clisock < 0)
    {
        perror("accept error");
        exit(-1);
    }

    fd_set readset;
    fd_set exceptset;
    FD_ZERO(&readset);
    FD_ZERO(&exceptset);
    FD_SET(clisock, &readset);
    FD_SET(clisock, &exceptset);

    while(1)
    {
        int ret = select(clisock + 1, &readset, NULL, &exceptset, NULL);
        if(ret < 0)
        {
            perror("select error");
            exit(-1);
        }

        for(int i = 0; i < clisock + 1; ++i)
        {
            if(FD_ISSET(i, &exceptset))
            {
                // 带外数据
                char buf[1024];
                int ret = recv(i, buf, 1024, MSG_OOB);
                if(ret < 0)
                {
                    perror("recv error");
                    exit(-1);
                }

                cout << "带外数据: " << buf << endl;
            }
            else if(FD_ISSET(i, &readset))
            {
                // 普通数据
                char buf[1024];
                int ret = recv(i, buf, 1024, 0);
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
