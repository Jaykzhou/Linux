// 实现进程间共享内存的同步与互斥
// 使用互斥锁 / 文件锁来实现共享内存的互斥工作
// 使用信号 / 信号量来实现共享内存的同步工作
// 当前使用互斥锁与信号来实现共享内存的同步与互斥工作
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
using namespace std;

#define FILENAME "./file.txt"
#define ID 'a'
#define SIZE 4096

int main()
{
    // IPC 标识符
    int key = ftok(FILENAME, ID);
    if(key < 0)
    {
        perror("ftok error!");
        exit(-1);
    }

    // 创建共享内存
    int shmid = shmget(key, 0, 0664);
    if(shmid < 0)
    {
        perror("shmget error!");
        exit(-1);
    }

    // 映射共享内存
    void* addr = shmat(shmid, NULL, 0);

    // 获取互斥锁
    pthread_mutex_t* sharemutex;
    sharemutex = (pthread_mutex_t*)addr;

    // 先加锁，后访问
    char buf[40960];
    pthread_mutex_lock(sharemutex);
    memcpy(buf, (char*)addr + sizeof(pthread_mutex_t), 40960);
    pthread_mutex_unlock(sharemutex);
    cout << buf << endl;

    // 操作完毕，取消映射
    int ret = shmdt(addr);
    if(ret < 0)
    {
        perror("shmdt error!");
        exit(-1);
    }

    // 访问完毕，删除共享内存
    //shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
