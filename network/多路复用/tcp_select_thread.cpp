// 多路转接并发服务器(Tcp)
#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <map>
using namespace std;

#define SOCKADDR "192.168.248.129"
#define SOCKPORT 8888
#define SEC 2

struct client 
{
    int cfd;
    void* data;     // 客户端相关数据
};

void* handle(void* arg)
{
    struct client* cli = (struct client*)arg;
    while(1)
    {
        char buf[1024];
        bzero(buf, 1024);
        int ret = read(cli->cfd, buf, 1024);
        if(ret < 0)
        {
            perror("read error");
            close(cli->cfd);
            return NULL;
        }
        else if(ret == 0)
        {
            cout << "peer shutdown!" << endl;
            close(cli->cfd);
            return NULL;
        }
        cout << "----------------------Client Message: " << buf;

        char* sdata = "Hello, You are great today!\n";
        write(cli->cfd, sdata, strlen(sdata));
    }

    return NULL;
}

// 封装 Select
class Select 
{
    public:
        Select() :maxfd(-1) {
            FD_ZERO(&readset);
        }

        bool Addevent(int fd)
        {
            FD_SET(fd, &readset);
            if(fd >= maxfd)
                maxfd = fd + 1;

            return true;
        }

        bool Delevent(int fd)
        {
            FD_CLR(fd, &readset);
            for(int i = maxfd; i >= 0; --i)
            {
                if(FD_ISSET(i, &readset))
                {
                    maxfd = i + 1;
                    return true;
                }
            }

            maxfd = -1;
            return true;
        }

        bool Waitevent(vector<int>& readlist, int sec)
        {
            fd_set temp = readset;
            struct timeval time;
            time.tv_sec = sec;
            int ret  =select(maxfd, &temp, NULL, NULL, &time);
            if(ret < 0)
            {
                perror("select error");
                exit(-1);
            }
            else if(ret == 0)
            {
                cout << "wait timeout!" << endl;
            }

            // 事件结合中遍历寻找就绪事件
            for(int i = 0; i < maxfd; ++i)
            {
                if(FD_ISSET(i, &temp))
                {
                    readlist.push_back(i);
                }
            }

            return true;
        }

    private:
        fd_set readset;
        int maxfd;
};

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd < 0) perror("socket error"), exit(-1);

    int opt = 1; 
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(ret < 0) perror("setsockopt error"), exit(-1);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SOCKPORT);
    inet_pton(AF_INET, SOCKADDR, (void*)&saddr.sin_addr);
    ret = bind(lfd, (struct sockaddr*)& saddr, sizeof(saddr));
    if(ret < 0) perror("bind error"), exit(-1);

    ret = listen(lfd, 128);
    if(ret < 0) perror("bind error"), exit(-1);

    // Select 监控监听套接字
    Select s;
    s.Addevent(lfd);

    map<int, struct client> climasg;         // 保存所有客户端的 socket, 为每个 socket 创建一个线程为其服务。
    while(1)
    {
        vector<int> client;
        s.Waitevent(client, SEC);

        for(auto c : client)
        {
            // 处理监听套接字
            if(c == lfd)
            {
                struct sockaddr_in cliaddr;
                socklen_t clilen = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
                if(cfd < 0) perror("accept error"), exit(-1);

                char buf[1024];
                bzero(buf, 1024);
                inet_ntop(AF_INET, (void*)&cliaddr.sin_addr, buf, INET_ADDRSTRLEN);
                cout << "Recv a message, client IP: " << buf << " ";
                cout << "Port: " << ntohs(cliaddr.sin_port) << endl;

                s.Addevent(cfd);
            }
            else 
            {
                // 在红黑树中查找是否已经为当前socket创建了线程
                if(climasg.find(c) == climasg.end())
                {
                    // 有新客户端的数据，创建线程为其服务
                    pthread_t tid;
                    struct client cli;
                    cli.cfd = c;
                    climasg[c] = cli; 
                    pthread_create(&tid, NULL, handle, (void*)&climasg[c]);
                    pthread_detach(tid);
                }
                else 
                {
                    // 原有线程自动处理老客户端的数据，不用再创建新的线程
                }
            }
        }
    }

    return 0;
}
