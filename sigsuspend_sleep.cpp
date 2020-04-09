// 使用sigsuspend函数解决时序竞争的问题
// 目标: 实现稳定性更高的sleep函数
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

void handler(int signo)
{
    return;
}

int Sleep(int sec)
{
    // 捕捉信号
    struct sigaction newact, oldact;
    newact.sa_handler = handler;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    int ret = sigaction(SIGALRM, &newact, &oldact);
    if(ret < 0)
    {
        perror("sigaction error!");
        exit(-1);
    }

    // 屏蔽 SIGALRM 信号
    sigset_t newmask, oldmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    // 定时器
    int retsec = alarm(sec);

    // sigsuspend函数期间使用临时的susmask，不屏蔽SIGALRM信号，屏蔽其他信号，因此当前只能接受到SIGALRM信号。
    sigset_t susmask = oldmask;
    sigdelset(&susmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &susmask, NULL);
    sigsuspend(&susmask);
    
    // 清空计时器
    alarm(0);

    // 恢复原有的信号屏蔽字 
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    // 恢复原有的 SIGALRM 处理动作
    sigaction(SIGALRM, &oldact, NULL);

    return retsec;
}

int main()
{
    while(1)
    {
        cout << "hello world!" << endl;
        Sleep(1);
    }

    return 0;
}
