// Select多路转接的Tcp服务器
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <vector>
using namespace std;

#define PORT 8888
#define ADDR "192.168.248.129"

// 封装一个Select类
class Select 
{
    public:
        Select() :maxfd(-1)
        {
            FD_ZERO(&readfd);
        }

        bool Addevent(int fd)
        {
            FD_SET(fd, &readfd);
            if(fd >= maxfd)
                maxfd = fd + 1;
            return true;
        }

        bool Delevent(int fd)
        {
            FD_CLR(fd, &readfd);
            for(int i = maxfd; i >= 0; --i)
            {
                if(FD_ISSET(i, &readfd))
                {
                    maxfd = i + 1;
                    return true;
                }
            }

            maxfd = -1;
            return true; 
        }

        bool Cleevent()
        {
            FD_ZERO(&readfd);
            return true;
        }

        bool Waitevent(vector<int>& clist)
        {
            fd_set rset = readfd;               // 就绪时间将会被移除，因此需要每次进行添加。

            struct timeval t;
            t.tv_sec = 1;
            int num = select(maxfd, &rset, NULL, NULL, &t);
            if(num < 0)
            {
                perror("select error!");
                exit(-1);
            }
            else if(num == 0)
            {
                cout << "wait timeout!" << endl;
            }
            else 
            {
                for(int i = 0; i < maxfd; ++i)
                {
                    if(FD_ISSET(i, &rset))
                    {
                        clist.push_back(i);
                    }
                }
            }

            return true;
        }

    private:
        fd_set readfd;
        int maxfd;                      // 最大文件描述符 + 1
};

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd < 0)
    {
        perror("socket error!");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ADDR, (void*)&saddr.sin_addr);
    int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret < 0)
    {
        perror("bind error!");
        exit(-1);
    }

    ret = listen(lfd, 128);
    if(ret < 0)
    {
        perror("listen error!");
        exit(-1);
    }

    // select 监听
    Select s;
    s.Addevent(lfd);
    while(1)
    {
        vector<int> clist;
        s.Waitevent(clist);
        for(auto e : clist)
        {
            if(e == lfd)
            {
                // 监听套接字
                struct sockaddr_in caddr;
                socklen_t socklen = sizeof(caddr);
                int cfd = accept(lfd, (struct sockaddr*)&caddr, &socklen);
                s.Addevent(cfd);
            }
            else 
            {
                // 客户端套接字
                char buf[1024];
                bzero(buf, 1024);
                int ret = read(e, buf, 1024);
                cout << buf << endl;

                for(int i = 0; i < ret; ++i)
                {
                    buf[i] = toupper(buf[i]);
                }
                write(e, buf, ret);
            }
        }
    }
    
    return 0;
}
