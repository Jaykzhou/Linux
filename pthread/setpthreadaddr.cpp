// 自定义栈地址，方便创建更多数量的线程
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

#define SIZE 2048000000 

void* handle(void* arg)
{
    while(1)
        sleep(1);
}

int main()
{
    // 创建大量线程
    int count = 0;
    while(1)
    {
        // 设置栈的空间地址
        void* addr = malloc(SIZE);
        if(addr == NULL)
        {
            perror("malloc error!");
            exit(-1);
        }
        pthread_attr_t pattr;
        pthread_attr_init(&pattr);
        pthread_attr_setstack(&pattr, addr, SIZE);

        pthread_t tid;
        int ret = pthread_create(&tid, &pattr, handle, NULL);
        if(ret != 0)
        {
            perror("pthread_create");
            exit(-1);
        }
        pthread_detach(tid);

        count++;
        cout  << "----------------" << count << endl;
    }

    return 0;
}
