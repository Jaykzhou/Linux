// 互斥锁保证线程间的互斥操作
// 主线程打印: HeLLO WORLD!
// 其他线程打印: hello world!
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* pthread_handle(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        cout << "hello ";
        sleep(1);
        cout << "world!" << endl;
        pthread_mutex_unlock(&mutex);
        
        usleep(100);
    }

    return NULL;
}

int main()
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, pthread_handle, NULL);
    if(ret != 0)
    {
        perror("create error!");
        exit(-1);
    }
    pthread_detach(tid);

    while(1)
    {
        pthread_mutex_lock(&mutex);
        cout << "HELLO ";
        sleep(1);
        cout << "WORLD!" << endl;
        pthread_mutex_unlock(&mutex);

        usleep(100);
    }

    return 0;
}
