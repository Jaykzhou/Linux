// 进程间通信 --> 共享内存
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
using namespace std;

#define FILENAME "./fileshm.txt"     // 文件路径
#define ID 'a'                       // 项目ID
#define SIZE 4096

int main()
{
    // IPC 标识符
    int ipcid = ftok(FILENAME, ID);

    // 创建共享内存
    int shmid = shmget(ipcid, SIZE * 10, IPC_CREAT | IPC_EXCL | 0664);
    if(shmid < 0)
    {
        cout << "共享内存创建失败！" << endl;
        perror("shmget error!");
        exit(-1);
    }

    // 映射共享内存
    char* addr = (char*)shmat(shmid, 0, 0);
    if(addr == NULL)
    {
        cout << "地址映射失败！" << endl;
        perror("shmat error!");
        exit(-1);
    }

    // 对内存进行操作(读写)
    char* buf = "Today is a beautiful day!";
    memcpy(addr, buf, 1024);

    // 取消映射
    shmdt(addr);
   
    //sleep(100);

    return 0;
}
