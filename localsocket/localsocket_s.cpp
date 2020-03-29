// 使用本地套接字进行进程之间通信
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstddef>
using namespace std;

#define WPATH "mywrite.socket"

int main()
{
    int wfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(wfd < 0) perror("socket error"), exit(-1);

    struct sockaddr_un waddr;
    waddr.sun_family = AF_UNIX;
    strcpy(waddr.sun_path, WPATH);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(WPATH);      // 求解地址长度

    unlink(WPATH);
    int ret = bind(wfd, (struct sockaddr*)&waddr, len);
    if(ret < 0) perror("bind error"), exit(-1);

    ret = listen(wfd, 128);
    if(ret < 0) perror("bind error"), exit(-1);

    while(1)
    {
        // 循环接受客户端发送的数据
        struct sockaddr_un raddr;
        socklen_t len = sizeof(raddr);
        int rfd = accept(wfd, (struct sockaddr*)&raddr, &len);
        if(rfd < 0) perror("accept error"), exit(-1);

        len -= offsetof(struct sockaddr_un, sun_path);
        cout << "client bind filename: " << raddr.sun_path << endl;

        while(1)
        {
            char buf[1024];
            bzero(buf, 1024);
            int ret = read(rfd, buf, 1024);
            if(ret < 0) perror("read error"), exit(-1);
            else if(ret == 0) 
            {
                cout << "peer shutdown!" << endl;
                break;
            }
            cout << buf << endl;

            ret = write(rfd, buf, ret);
            if(ret < 0) perror("read error"), exit(-1);
        }
    }

    return 0;
}
