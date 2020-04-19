// 使用储存映射IO向文件中写数据
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
using namespace std;

#define FILENAME "./mmap.txt"
#define MODE 0664
#define PAGR 4096

int main()
{
    // 打开文件
    int fd = open(FILENAME, O_CREAT | O_RDWR, MODE);
    if(fd < 0)
    {
        perror("open error!");
        exit(-1);
    }

    // 拓展文件
    truncate(FILENAME, PAGR * 10);

    // 建立映射区
    void* addr = mmap(0, PAGR * 10, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == NULL)
    {
        perror("mmap error!");
        exit(-1);
    }

    // unlink(FILENAME);     调用unlink删除目录项，inode结点并不会直接删除，而是等待当前进程不在使用时，内核才释放该文件信息

    // 写入
    char buf[1024] = "hello world, change world!\n";
    memcpy(addr, buf, 1024);
    char buf2[1024] = "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhh!\n";
    memcpy((char*)addr + 1024, buf2, 1024);
    char buf3[1024] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!\n";
    memcpy((char*)addr + PAGR, buf3, 1024);
    msync(addr, PAGR * 10, MS_ASYNC);                      // 刷新缓冲区

    // 读取
    char data[1024];
    bzero(data, 1024);
    memcpy(data, addr, 1024);
    cout << "读取的数据为: " << data;

    // 取消映射区
    munmap(addr, PAGR * 10);

    // 关闭文件
    close(fd);

    return 0;
}
