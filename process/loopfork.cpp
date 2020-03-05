// 创建 N 个子进程
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

void sys_err(char* str)
{
    perror(str);
    exit(-1);
}

int main()
{
    pid_t pid = 0;
    int i = 0;
    for(i = 0; i < 5; ++i)
    {
        pid = fork();
        if(pid < 0)
            sys_err("fork error");
        else if(pid == 0)             // 子进程直接退出循环，防止干扰进程的创建
            break;
    }

    if(i >= 5)
        cout << getpid() << " is father!" << endl;
    else 
        cout << getpid() << " is child!" << endl;
    // 父进程只有循环 5 次之后才会退出，因此当 i >= 5 退出的是父进程，其他情况下退出的都是创建好的子进程
    // 子进程，父进程创建完毕之后，它们会与 bash 进程一起抢夺 CPU 的资源，因此我们每次看到的情况并不一样
    return 0;
}
