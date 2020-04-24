// 设置线程栈的空间地址以及设置线程栈的大小
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

#define SIZE 20380 

void* pthread_handle(void* arg)
{
    while(1)
        sleep(1);
}

int main()
{
    // 创建大量线程
    for(int i = 0; i < 10; ++i)
    {
        // 设置线程属性
        pthread_attr_t pattr;
        pthread_attr_init(&pattr);
        pthread_attr_setstacksize(&pattr, SIZE);

        pthread_t tid;
        int ret = pthread_create(&tid, &pattr, pthread_handle, NULL);
        if(ret != 0)
        {
            perror("pthread_create error!");
            exit(-1);
        }
        pthread_detach(tid);

        // 获取一下当前线程的属性
        size_t size = 0;
        pthread_attr_getstacksize(&pattr, &size);
        cout << "线程栈大小: " << size << endl;

        pthread_attr_destroy(&pattr);
    }

    return  0;
}
