// 熟悉异步I/O操作
//  1. 捕捉SIGIO信号
//  2. 设置接受SIGIO信号得进程以及进程组
//  3. 设置文件的异步状态标识，使得该文件可以进行异步I/O
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
using namespace std;

void RecvMessage(int signo)
{
    if(signo == SIGIO)
    {
        char buf[1024];
        bzero(buf, 1024);
        read(0, buf, 1024);
        cout << "普通数据到达！" << endl;
        cout << "标准输入发送的数据为: " << buf;
    }
    else 
    {
        // SIGURG
    }
}

int main()
{
    // 捕捉SIGIO信号
    struct sigaction sig;
    sig.sa_handler = RecvMessage;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;        // 重启慢速系统调用
    sigaction(SIGIO, &sig, NULL);     // 不关心原有信号处理方式

    // 设置接受SIGIO信号的进程
    fcntl(0, F_SETOWN, getpid());

    // 设置文件状态标记
    int flag = fcntl(0, F_GETFL);
    flag |= O_ASYNC;                  
    fcntl(0, F_SETFL, flag);

    while(1);

    return 0;
}
