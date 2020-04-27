// 测试读写锁的使用场景
// 3 个线程去写共享资源
// 5 个线程去读共享资源
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

// 共享资源
int num = 0;

// 读写锁
pthread_rwlock_t rwlock;

// 写线程
void* writethread(void* arg)
{
    while(1)
    {
        pthread_rwlock_wrlock(&rwlock);
        num += 2;
        cout << "当前资源已经被更改为: " << num << endl;
        pthread_rwlock_unlock(&rwlock);
        
        int sleeptime = rand() % 3;
        sleep(sleeptime);
    }

    return NULL;
}

// 读线程
void* readthread(void* arg)
{
    while(1)
    {
        pthread_rwlock_rdlock(&rwlock);
        cout << "------------------读到的数据为: " << num << endl;
        pthread_rwlock_unlock(&rwlock);

        int sleeptime = rand() % 2;
        sleep(sleeptime);
    }

    return NULL;
}

int main()
{
    srand(time(0));

    // 初始化锁资源
    pthread_rwlock_init(&rwlock, NULL);

    // 写线程
    pthread_t wtid[3];
    for(int i = 0; i < 3; ++i)
    {
        pthread_create(&wtid[i], NULL, writethread, NULL);
    }

    // 读线程
    pthread_t rtid[5];
    for(int i = 0; i < 5; ++i)
    {
        pthread_create(&rtid[i], NULL, readthread, NULL);
    }

    // 回收线程
    for(int i = 0; i < 3; ++i)
    {
        pthread_join(wtid[i], NULL);
    }

    for(int i = 0; i < 5; ++i)
    {
        pthread_join(rtid[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);

    return 0;
}
