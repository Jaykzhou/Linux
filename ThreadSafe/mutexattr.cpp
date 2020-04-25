// 设置检错锁，解决最简单的死锁情况发生
// 同一线程重复加同一把锁
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
using namespace std;

int main()
{
    // 设置检错锁
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

    // 初始化互斥锁
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, &attr);

    int count = 0;
    while(1)
    {
        int ret = pthread_mutex_lock(&mutex);
        if(ret != 0)
        {
            perror("pthread_mutex_lock error!");
            exit(-1);
        }
        cout << "hello world!" << endl;

        count++;
        if(count == 5)
        {
            int ret = pthread_mutex_lock(&mutex);
            if(ret != 0)
            {
                cout << "线程已持有该锁!" << endl;
                exit(-1);
            }
        }
        
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&mutex);

    return 0;
}
