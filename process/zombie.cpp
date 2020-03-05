// 僵尸进程 -> 回收僵尸进程
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

void sys_err(char* str)
{
    perror(str);
    exit(-1);
}

void handle(int signo)
{
    if(signo == SIGCHLD)
    {
        int states = 0;
        int ret = wait(&states);          // 有可能会发生系统中断
        if(ret < 0)
            sys_err("wait error:");
        
        if(WIFEXITED(states))
        {
            states = WEXITSTATUS(states);
            cout << "recv SIGCHLD: " << getpid() << " " << states << endl;
        }
    }
    else if(signo == SIGINT)
    {
        exit(-1);
    }
}

void childfun()
{
    signal(SIGINT, handle);
    while(1)
    {
        cout << "playing music!" << endl;
        sleep(1);
    }
}

void fatherfun()
{
    signal(SIGINT, SIG_IGN);
    while(1)
    {
        cout << "working!" << endl;
        sleep(1);
    }
}


int main()
{
    // 自定义处理 SIGCHLD 信号处理方式
    struct sigaction act, oldact;
    act.sa_handler = handle;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, &oldact);

    pid_t pid = 0;
    pid = fork();
    if(pid < 0)
        sys_err("fork error");
    else if(pid == 0)
    {
        // child
        childfun();
    }
    else 
    {
        // father
        fatherfun();
    }

    return 0;
}
