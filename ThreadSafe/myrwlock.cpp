// 使用互斥锁与条件变量模拟实现读写锁。
// 读共享，写互斥，写者优先加锁。
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

// 读写锁
class rwlock
{
    public:
        ~rwlock()
        {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&rcond);
            pthread_cond_destroy(&wcond);
        }

        // 初始化锁属性
        void rwlock_init()
        {
            flag = 0;
            rcount = 0;
            wcount = 0;

            pthread_mutex_init(&mutex, NULL);
            pthread_cond_init(&rcond, NULL);
            pthread_cond_init(&wcond, NULL);
        }

        // 读锁
        void rwlock_rlock()
        {
            pthread_mutex_lock(&mutex);
            while(wcount > 0 || flag == 1)           // 当前写锁存在 / 当前有线程加写锁
            {
                pthread_cond_wait(&rcond, &mutex);
            }

            // 加读锁成功
            rcount++;
            pthread_mutex_unlock(&mutex);
        }

        // 解读锁
        void rwlock_unrlock()
        {
            pthread_mutex_lock(&mutex);
            rcount--;
            if(rcount == 0)                          // 当前没有线程加锁
            {
                if(flag == 1)                        // 当前有线程加写锁
                {
                    pthread_cond_broadcast(&wcond);
                }
                else 
                {
                    pthread_cond_broadcast(&rcond);
                }
            }
            pthread_mutex_unlock(&mutex);
        }

        // 写锁
        void rwlock_wlock()
        {
            pthread_mutex_lock(&mutex);
            flag = 1;
            while(wcount + rcount > 0)
            {
                pthread_cond_wait(&wcond, &mutex);
            }

            // 加写锁成功
            flag = 0;
            wcount++;
            pthread_mutex_unlock(&mutex);
        }

        // 解写锁
        void rwlock_unwlock()
        {
            pthread_mutex_lock(&mutex);
            wcount--;
            if(wcount == 0)
            {
                if(flag == 1)
                {
                    pthread_cond_broadcast(&wcond);
                }
                else 
                {
                    pthread_cond_broadcast(&rcond);
                }
            }
            pthread_mutex_unlock(&mutex);
        }

    private:
        pthread_mutex_t mutex;          // 互斥锁
        pthread_cond_t rcond;           // 读条件变量
        pthread_cond_t wcond;           // 写条件变量
        int flag;                       // 标识当前是否有线程加写锁
        int rcount;                     // 读者计数
        int wcount;                     // 写者计数
};

// 共享资源
int num = 0;

// 读写锁
rwlock rwlock;

// 写线程
void* writethread(void* arg)
{
    while(1)
    {
        rwlock.rwlock_wlock();
        num += 2;
        cout << "当前资源已经被更改为: " << num << endl;
        rwlock.rwlock_unwlock();
        
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
        rwlock.rwlock_rlock();
        cout << "------------------读到的数据为: " << num << endl;
        rwlock.rwlock_unrlock();

        int sleeptime = rand() % 2;
        sleep(sleeptime);
    }

    return NULL;
}

int main()
{
    srand(time(0));

    // 初始化锁资源
    rwlock.rwlock_init();

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

    return 0;
}
