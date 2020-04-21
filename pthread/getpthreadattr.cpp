// 获取一下线程常用的默认属性
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

int main()
{
    // 初始化线程的状态
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int pthreadstates = 0;
    pthread_attr_getdetachstate(&attr, &pthreadstates);
    if(pthreadstates == PTHREAD_CREATE_DETACHED)
    {
        cout << "线程属性为分离态!" << endl;
    }
    else if(pthreadstates == PTHREAD_CREATE_JOINABLE)
    {
        cout << "线程属性为加入态!" << endl;
    }

    // 获取线程的默认栈大小
    size_t stacksize = 0;
    pthread_attr_getstacksize(&attr, &stacksize);
    cout << "线程的默认栈大小为: " << stacksize << endl;

    return 0;
}
