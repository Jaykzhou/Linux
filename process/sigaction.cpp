// 使用 sigaction 函数自定义信号处理方式
#include <iostream>
#include <unistd.h>
#include <signal.h>
using namespace std;

void handle(int signo)
{
    if(signo == SIGINT) 
        cout << "recv SIGINT!" << endl;

    // 在执行 SIGINT 自定义信号处理时，屏蔽掉 SIGQUIT 信号，防止其他信号干扰 SIGINT 的信号处理方式
    sleep(10);
}

int main()
{
    struct sigaction act, oldact;
    act.sa_handler = handle;
    
    // 处理 sa_mask 临时信号屏蔽字
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, &oldact);

    while(1);
    return 0;
}
