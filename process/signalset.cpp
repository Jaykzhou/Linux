// 练习信号操作集函数:
// 1. 将信号进行阻塞操作
// 2. 查看信号集当中的位标记
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

// 1. 创建自定义信号
sigset_t set, oldset, pending;

void print()
{
    for(int i = 1; i < 32; ++i)
    {
        if(sigismember(&pending, i))
            cout << '1';
        else 
            cout << '0';
    }
    cout << endl;                      // 换行 + 刷新缓冲区
}

void handle(int signo)
{
    if(signo == SIGUSR1)
        sigprocmask(SIG_UNBLOCK, &set, NULL);
}

int main()
{
    // 使用 SIGUSR1 来解除阻塞
    signal(SIGUSR1, handle);

    // 2. 清空信号集
    sigemptyset(&set);
    // 3. 加入需要阻塞的信号
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    // 4. 将自定义信号集加入到阻塞信号集mask中
    sigprocmask(SIG_BLOCK, &set, &oldset);

    while(1)
    {
        // 5. 获取未决信号集
        sigpending(&pending);
        print();
        sleep(1);
    }

    return 0;
}
