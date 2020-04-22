// 测试取消线程时的取消点
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
using namespace std;

void* pthread_handler(void* arg)
{
    int count = 0;
    while(1)
    {
        count++;
        pthread_testcancel();           // 自行加入线程取消点
    }

    return NULL;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, pthread_handler, NULL);

    // 取消/杀死线程
    pthread_cancel(tid);

    // 回收子线程
    int* num[1]; 
    pthread_join(tid, (void**)num);
    cout << (int)num[0] << endl;

    return 0;
}
