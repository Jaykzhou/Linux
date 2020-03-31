// poll多路转接Tcp服务器
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <poll.h>
using namespace std;

#define SADDR "192.168.248.129"
#define PORT 8888
#define NUM 20

// 封装 poll 监控过程
class Poll 
{
    public:
        Poll(int n = NUM)
        {
            bzero(&eventfd, sizeof(eventfd));
            eventfd = (struct pollfd*)malloc(sizeof(eventfd) * n);
        }
        
        ~Poll()
        {
            free(eventfd);
        }

        bool Addevent(int fd, int event = POLLIN)
        {
            eventfd[index].fd = fd;
            eventfd[index].events = event;
            num++;
            index++;

            return true;
        }

        bool Waitevent(vector<int>& cfdlist, int sec, int event = POLLIN)
        {
            int ret = poll(eventfd, num, sec);
            if(ret < 0)
            {
                perror("poll error!");
                return false;
            }
            else if(ret == 0)
            {
                cout << "wait timeout!" << endl;
            }

            // 遍历整个eventfd寻找就绪事件
            for(int i = 0; i < (int)num; ++i)
            {
                if(eventfd[i].revents & event)
                {
                    // 有就绪事件发生
                    cfdlist.push_back(eventfd[i].fd);
                }
            }

            return true;
        }

    private:
        struct pollfd* eventfd;
        nfds_t num = 0;
        int index = 0;
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
    if(ret < 0) perror("bind error!"), exit(-1);

    int opt = 1;
    ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(ret < 0) perror("bind error!"), exit(-1);

    // 添加事件(默认可读)
    Poll pevent;
    pevent.Addevent(lfd);
    while(1)
    {
        vector<int> cfdlist;
        bool flag = pevent.Waitevent(cfdlist, 200);
        if(flag == false) continue;
        for(auto cfd : cfdlist)
        {
            if(cfd == lfd)
            {
                // 监听套接字就绪
                struct sockaddr_in caddr;
                socklen_t len = sizeof(caddr);
                int ret = accept(lfd, (struct sockaddr*)&caddr, &len);
                if(ret < 0) perror("accept error!"), exit(-1);

                char buf[1024];
                cout << "Recv a Client Ip: " << inet_ntop(AF_INET, (void*)&caddr.sin_addr, buf, 1024);
                cout << " Port: " << ntohs(caddr.sin_port) << endl;
                pevent.Addevent(ret);
            }
            else 
            {
                char buf[1024];
                int ret = read(cfd, buf, 1024);
                if(ret < 0) perror("read error!"), exit(-1);
                else if(ret == 0)
                {
                    cout << "peer shutdown!" << endl;
                    close(cfd);
                }

                cout << buf;

                char* data = "Today is a happy day!\n";
                write(cfd, data, strlen(data));
            }
        }
    }
        
    return 0;
}
