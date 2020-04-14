// 实现进程间共享内存的同步与互斥
// 使用互斥锁 / 文件锁来实现共享内存的互斥工作
// 使用信号 / 信号量来实现共享内存的同步工作
// 当前使用互斥锁与信号来实现共享内存的同步与互斥工作
// 未加锁问题:
//      1. 穿插写
//      2. 读取数据不完整的情况
// 解决方法:
//      1. 利用互斥锁实现共享内存数据访问的安全性
//      2. 通过共享共享内存来传递同一把互斥锁
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <pthread.h>
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
    int shmid = shmget(key, 10 * SIZE, IPC_CREAT | IPC_EXCL | 0664);
    if(shmid < 0)
    {
        perror("shmget error!");
        exit(-1);
    }

    // 映射共享内存
    void* addr = (char*)shmat(shmid, NULL, 0);

    // 创建互斥锁，并初始化互斥锁
    pthread_mutexattr_t attr;               
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);                // 设置锁属性 -> 进程间锁
    pthread_mutex_t* sharemutex = (pthread_mutex_t*)addr;
    pthread_mutex_init(sharemutex, &attr);

    // 进程间通信
    pthread_mutex_lock(sharemutex);                          // 加锁
    cout << "已获得锁！" << endl;
    int ntowrite = 0;
    while(ntowrite < 20480)
    {
        // 写入20480字节数据
        char* data = "hello world!";
        memcpy((char*)addr + sizeof(pthread_mutex_t) + ntowrite, data, strlen(data));
        ntowrite += strlen(data);

        cout << "已写入: " << ntowrite << " 字节数据！" << endl;

        usleep(500);
    }
    pthread_mutex_unlock(sharemutex);
    cout << "已释放锁！" << endl;
    cout << "写入完毕！" << endl;

    // 操作完毕，取消映射
    int ret = shmdt(addr);
    if(ret < 0)
    {
        perror("shmdt error!");
        exit(-1);
    }

    return 0;
}
