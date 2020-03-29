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
#define RPATH "myread.socket"

int main()
{
    int rfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(rfd < 0) perror("socket error"), exit(-1);

    struct sockaddr_un raddr;
    raddr.sun_family = AF_UNIX;
    strcpy(raddr.sun_path, RPATH);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(RPATH);      // 求解地址长度

    unlink(RPATH);
    int ret = bind(rfd, (struct sockaddr*)&raddr, len);
    if(ret < 0) perror("bind error"), exit(-1);

    struct sockaddr_un waddr;
    waddr.sun_family = AF_UNIX;
    strcpy(waddr.sun_path, WPATH);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(WPATH);          // 求解地址长度
    ret = connect(rfd, (struct sockaddr*)&waddr, len);
    if(ret < 0) perror("connect error"), exit(-1);

    while(1)
    {
        char buf[1024];
        bzero(buf, 1024);
        cin >> buf;
        int ret = write(rfd, buf, 1024);
        if(ret < 0) perror("read error"), exit(-1);

        ret = read(rfd, buf, 1024);
        if(ret < 0) perror("read error"), exit(-1);
        else if(ret == 0) 
        {
            cout << "peer shutdown!" << endl;
            break;
        }
        cout << buf << endl;
    }

    return 0;
}
