// Tcp 通讯客户端
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
using namespace std;

#define SPORT 8888
#define SADDR "192.168.248.129"

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket error");
        return -1;
    }

    // bind();
    
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SPORT);
    inet_pton(AF_INET, SADDR, (void*)&saddr.sin_addr);
    int ret = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret < 0)
    {
        perror("connect error");
        return -1;
    }

    while(1)
    {
        char buf[1024];
        bzero(buf, 1024);
        cin >> buf;
        write(sockfd, buf, strlen(buf));

        bzero(buf, 1024);
        read(sockfd, buf, 1024);
        cout << buf;
    }

    close(sockfd);

    return 0;
}
