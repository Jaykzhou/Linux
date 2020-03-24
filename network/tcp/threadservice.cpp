// 多线程服务器
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include "wrap.h"
using namespace std;

#define PROT 8888
#define ADDR "192.168.248.129"

struct client 
{
    int cfd;
    void* args;
};

void* handle(void* args)
{
    // 普通线程处理任务
    struct client* cli = (struct client*)args;
    while(1)
    {
        char buf[1024];
        bzero(buf, 1024);
        int ret = read(cli->cfd, buf, 1024);
        if(ret == 0) break;                     // 对端关闭，读取数据为 0

        for(int i = 0; i < ret; ++i)
            buf[i] = toupper(buf[i]);
        write(cli->cfd, buf, ret);
    }

    return nullptr;
}

int main()
{
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PROT);
    inet_pton(AF_INET, ADDR, (void*)&saddr.sin_addr);
    Bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
    Listen(lfd, 128);

    struct client cli[1024];
    bzero(cli, sizeof(struct client) * 1024);
    int i = 0;

    while(1)
    {
        struct sockaddr_in caddr;
        socklen_t lenth = sizeof(caddr);
        int cfd = Accept(lfd, (struct sockaddr*)&caddr, &lenth);

        // 创建线程进行任务处理
        pthread_t tid;
        cli[i].cfd = cfd;
        cli[i].args = nullptr;
        pthread_create(&tid, NULL, handle, (void*)&cli[i]);
        pthread_detach(tid);

        i++;
    }

    return 0;
}
