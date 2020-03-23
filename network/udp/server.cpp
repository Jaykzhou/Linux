// Udp服务器
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <strings.h>
#include "wrap.h"
using namespace std;

#define PORT 8888
#define IPADDR "192.168.248.129"

int main()
{
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IPADDR, (void*)&addr.sin_addr);
    Bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    while(1)
    {
        char buf[1024];
        bzero(buf, 1024);
        struct sockaddr_in caddr;
        socklen_t length = sizeof(caddr);
        int ret = recvfrom(sockfd, (void*)buf, 1024, 0, (struct sockaddr*)&caddr, &length);
        cout << "IP:" << caddr.sin_addr.s_addr << ", Port:" << caddr.sin_port << ", say:" << buf;

        sendto(sockfd, (void*)buf, ret, 0, (struct sockaddr*)&caddr, length);
    }

    return 0;
}
