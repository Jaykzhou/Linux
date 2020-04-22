// 测试当前系统一个进程中可以创建的最大线程数量
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <vector>
#include <unistd.h>
using namespace std;

void* pthread_handler(void* arg)
{
    while(1)        // 保证线程不退出
        sleep(1);

    return NULL;
}

int main()
{
    pthread_t tid;
    int count = 0;
    while(1)
    {
        int ret = pthread_create(&tid, NULL, pthread_handler, NULL);
        if(ret != 0)
        {
            perror("pthread_create error!");
            exit(-1);
        }
        pthread_detach(tid);

        count++;
        cout << count << endl;
    }

    return 0;
}
