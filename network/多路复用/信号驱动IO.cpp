// 体会信号驱动IO的工作方式
// 目标:
//     当标准输入有数据的时触发SIGIO信号，内核捕捉到信号后，采用信号回调机制将缓冲区中的读取数据之后输出到标准输出。
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
using namespace std;

void handle(int signo)
{
    cout << "触发SIGIO信号!" << endl;

    char buf[1024];
    bzero(buf, 1024);
    read(0, buf, 1024);
    cout << buf;
}

int main()
{
    signal(SIGIO, handle);

    // 设置接受SIGIO信号的进程
    fcntl(0, F_SETOWN, getpid());
    // 设置异步标记 O_ASYNC
    int flag = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, flag | O_ASYNC);

    while(1)
    {
        char buf[1024];
        bzero(buf, 1024);
        cin >> buf;
        sleep(1);
    }

    return 0;
}
