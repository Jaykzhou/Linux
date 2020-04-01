// poll多路复用Tcp服务器
#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>
using namespace std;

#define SADDR "192.168.248.129"
#define SPORT 8888
#define NUM 20

// 封装客户端数据
struct cfddata 
{
    void* data;
    int cfd;
};

// 封装Poll类
class Poll 
{
    public:
        Poll(int n = NUM)
        {
            events = (struct pollfd*)malloc(sizeof(struct pollfd) * n);
        }

        ~Poll()
        {
            free(events);
        }

        bool Addevent(int fd, int event = POLLIN)
        {
            events[num].fd = fd;
            events[num].events = event;
            num++;

            return true;
        }

        bool Waitevent(vector<int>& cfdlist, int sec, int event = POLLIN)
        {
            int ret = poll(events, num, sec);
            if(ret < 0)
            {
                perror("poll error!");
                exit(-1);
            }
            else if(ret == 0)
            {
                cout << "wait timeout!" << endl;
            }

            // 遍历寻找就绪事件
            for(int i = 0; i < num; ++i)
            {
                if(events[i].revents & event)
                {
                    cfdlist.push_back(events[i].fd);
                }
            }

            return true;
        }

    private:
        struct pollfd* events;
        int num = 0;
};

void* handle(void* arg)
{
    struct cfddata* cfddata = (struct cfddata*)arg;
    while(1)
    {
        char buf[1024];
        int ret = read(cfddata->cfd, buf, 1024);
        if(ret < 0) perror("read error!"), exit(-1);
        else if(ret == 0)
        {
            cout << "peer shutdown!" << endl;
            close(cfddata->cfd);       // 最好的方式是从红黑树中删除该条记录，方便下次通信(简易版)     
            return NULL;
        }
        
        cout << buf;

        ret = write(cfddata->cfd, buf, ret);
        if(ret < 0) perror("write error!"), exit(-1);
    }

    return NULL;
}

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd < 0) perror("socket error!"), exit(-1);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SPORT);
    inet_pton(AF_INET, SADDR, (void*)&saddr.sin_addr);
    int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret < 0) perror("bind error!"), exit(-1);

    ret = listen(lfd, 128);
    if(ret < 0) perror("bind error!"), exit(-1);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 添加监听事件
    Poll p;
    p.Addevent(lfd, POLLIN);
    map<int, struct cfddata> cfdhash;       
    while(1)
    {
        vector<int> cfdlist;
        p.Waitevent(cfdlist, 200, POLLIN);
        for(auto cfd : cfdlist)
        {
            if(cfd == lfd)
            {
                struct sockaddr_in caddr;
                socklen_t len = sizeof(caddr);
                int fd = accept(lfd, (struct sockaddr*)&caddr, &len);
                if(fd < 0) perror("accept error!"), exit(-1);
                char buf[1024];
                cout << "Recv a Client Ip: " << inet_ntop(AF_INET, (void*)&caddr.sin_addr, buf, 1024);
                cout << " Port: " << ntohs(caddr.sin_port) << endl;

                p.Addevent(fd, POLLIN);
            }
            else 
            {
                if(cfdhash.find(cfd) == cfdhash.end())
                {
                    // 为新的客户端建立一个通信线程
                    pthread_t tid;
                    struct cfddata arg;         
                    arg.cfd = cfd;
                    cfdhash[cfd] = arg;     // 防止文件描述符出当前作用域后，文件描述符被释放。
                    pthread_create(&tid, NULL, handle, (void*)&cfdhash[cfd]);
                    pthread_detach(tid);
                }
                else 
                {
                    // 原有客户端发送的数据继续使用原来的线程进行通信(不退出)
                }
            }
        }
        
    }

    return 0;
}
