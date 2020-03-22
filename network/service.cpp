// 一个简易版的服务器
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
using namespace std;

#define SPORT 8888
#define SADDR "192.168.248.129"

int main()
{
    // 创建套接字
    int sfd = -1;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0)
    {
        perror("socket error!");
        exit(-1);
    }

    // 绑定地址与端口(网络字节序)
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SPORT);
    inet_pton(AF_INET, SADDR, (void*)&saddr.sin_addr);
    int ret = bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret < 0)
    {
        perror("bind error");
        exit(-1);
    }

    // 监听端口
    ret = listen(sfd, 128);
    if(ret < 0)
    {
        perror("listen error");
        exit(-1);
    }

    // 获取已完成连接
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    int cfd = accept(sfd, (struct sockaddr*)&caddr, &clen);
    if(cfd < 0)
    {
        perror("accept error");
        exit(-1);
    }

    // 与客户端进行通信
    while(1)
    {
        char buf[BUFSIZ];
        read(cfd, buf, BUFSIZ);
        cout << buf << endl;

        char* str = "what are you doing?\n";
        write(cfd, str, strlen(str));
    }

    close(sfd);
    
    return 0;
}
