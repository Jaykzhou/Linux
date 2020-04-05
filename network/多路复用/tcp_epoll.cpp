// epoll多路转接epoll服务器
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <strings.h>
using namespace std;

#define EVENT_SIZE 1024
#define PORT 8888
#define SADDR "192.168.248.129"


// 封装 epoll 操作
class epoll 
{
    public:
        epoll(int n = EVENT_SIZE)
        {
            epfd = epoll_create(n);
            if(epfd < 0)
            {
                perror("epoll create error!");
                exit(-1);
            }

            // 为事件申请足够大的空间
            events = (epoll_event*)malloc(sizeof(epoll_event) * n);
        }

        ~epoll()
        {
            close(epfd);
            free(events);
        }

        bool Addevent(int fd, int event = EPOLLIN)
        {
            events[index].events = event;
            events[index].data.fd = fd;
            int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &events[index]);
            if(ret < 0) perror("epoll_ctl error!"), exit(-1);
            index++;

            return true;
        }

        bool Waitevent(vector<int>& cfdlist, int timeout = 200)
        {
            int ret = epoll_wait(epfd, events, index, timeout);
            if(ret < 0) perror("epoll_wait error!"), exit(-1);
            if(ret == 0)
            {
                cout << "wait timeout!" << endl;
            }

            // 返回所有就绪的文件描述符
            for(int i = 0; i < ret; ++i)
            {
                int cfd = events[i].data.fd;
                cfdlist.push_back(cfd);
            }

            return true;
        }

    private:
        int epfd = 0;
        int index = 0;
        epoll_event* events;
};

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd < 0) perror("socket error!"), exit(-1);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SADDR, (void*)&saddr.sin_addr);
    int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret < 0) perror("bind error!"), exit(-1);

    ret = listen(lfd, 128);
    if(ret < 0) perror("listen error!"), exit(-1);

    // 添加监听套接字
    epoll ep;
    ep.Addevent(lfd, EPOLLIN);
    cout << "Wait client connect ..." << endl;
    while(1)
    {
        vector<int> cfdlist;
        ep.Waitevent(cfdlist);
        for(auto fd : cfdlist)
        {
            if(fd == lfd)
            {
                struct sockaddr_in caddr;
                socklen_t len = sizeof(caddr);
                int cfd = accept(lfd, (struct sockaddr*)&caddr, &len);
                if(cfd < 0) perror("accept error!"), exit(-1);

                ep.Addevent(cfd);
            }
            else 
            {
                char buf[1024];
                bzero(buf ,1024);
                int ret = read(fd, buf, 1024);
                if(ret < 0) perror("read error!"), exit(-1);
                if(ret == 0) 
                {
                    cout << "peer shutdown!" << endl;
                    break;
                }

                // 回写
                write(fd, buf, ret);
            }
        }
    }

    return 0;
}
