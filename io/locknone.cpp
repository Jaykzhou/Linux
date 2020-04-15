// 多进程同时操作一个文件
// 当前进程的任务:
// 1. 取出文件中的数字
// 2. 使用其中的数字
// 3. 将数字加1，写回到文件中
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fcntl.h>
using namespace std;

#define FILENAME "./fileseq.txt"
#define MODE 0664

int mylock(int fd)
{
    return 0;
}

int myunlock(int fd)
{
    return 0;
}

int main()
{
    // 打开文件
    int fd = open(FILENAME, O_CREAT | O_RDWR, MODE);
    if(fd < 0)
    {
        perror("open error!");
        exit(-1);
    }

    char buf[1024];
    stringstream ss;
    stringstream ss1;

    // 循环执行 20000 次
    for(int i = 0; i < 20000; ++i)
    {
        mylock(fd);
        lseek(fd, 0, SEEK_SET);
        read(fd, &buf, 1023);
        buf[1023] = '\0';

        // 将字符串转化为数字
        int num = 0;
        ss << buf;
        ss >> num;
        num++;
        ss1 << num;
        ss1 >> buf;
        cout << buf << endl;

        lseek(fd, 0, SEEK_SET);
        write(fd, &buf, 1);
        myunlock(fd);
    }

    close(fd);
    
    return 0;
}
