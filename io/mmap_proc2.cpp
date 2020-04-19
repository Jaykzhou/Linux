// 使用存储映射I/O进行进程间通信
// 步骤:
//   1. 使用信号量保证进程间的互斥
//   2. 使用一个进程来向映射区写数据，另一个进程来从映射区读数据
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

#define FILENAME "./mmapproc.txt"
#define MODE 0664
#define PAGE 4096

int main()
{
    int fd = open(FILENAME, O_CREAT | O_RDWR, MODE);
    if(fd < 0)
    {
        perror("open error!");
        exit(-1);
    }

    void* addr = mmap(0, PAGE * 10, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == NULL)
    {
        perror("mmap error!");
        exit(-1);
    }

    // 获取信号量
    sem_t* sem = (sem_t*)addr;

    // 通信
    sem_wait(sem);         // -1 操作
    cout << "加锁成功！" << endl;

    char buf[1024];
    bzero(buf, 1024);
    memcpy(buf, (char*)addr + sizeof(sem_t), 1024);

    sem_post(sem);         // +1 操作
    cout << "解锁成功！" << endl;
    cout << buf << endl;

    sem_destroy(sem);
    munmap(addr, PAGE * 10);

    return 0;
}
