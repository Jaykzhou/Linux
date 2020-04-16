// 多进程同时操作一个文件
// 当前进程的任务:
// 1. 取出文件中的数字
// 2. 使用其中的数字
// 3. 将数字加 1，写回到文件中
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fcntl.h>
#include <strings.h>
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
    int fd = open(FILENAME, O_CREAT | O_RDWR, MODE);
    if(fd < 0)
    {
        perror("open error!");
        exit(-1);
    }

    int num = 0;
    stringstream sstream;
    char buf[1024];

    // 循环执行 20000 次
    for(int i = 0; i < 20000; ++i)
    {
        mylock(fd);
        lseek(fd, 0, SEEK_SET);
        read(fd, &buf, 1023);

        // 对序号进行 +1 操作
        sstream.str("");
        sstream.clear();        // 每次操作完毕后，需要清空当前流状态，防止到达流末尾(eofbit)，影响下一次的使用
        sstream << buf;
        sstream >> num;
        num++;

        sstream.str("");
        sstream.clear();        // 每次操作完毕后，需要清空当前流状态，防止到达流末尾(eofbit)，影响下一次的使用
        bzero(buf, 1024);
        sstream << num;
        sstream >> buf;

        lseek(fd, 0, SEEK_SET);
        write(fd, &buf, 1024);
        myunlock(fd);
    }

    close(fd);
    
    return 0;
}
