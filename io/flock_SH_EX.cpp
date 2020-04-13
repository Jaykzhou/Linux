// 测试锁在切换的过程中不是原子操作(共享 -> 互斥)
// 在切换期间，需要先释放原来的共享锁，然后再加上互斥锁，在中间的间隙时间内有可能会被其他阻塞的进程加锁成功。
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#define FILENAME "./file.txt"

int main()
{
    int lockfd = open(FILENAME, O_CREAT | O_RDWR, 0664);
    if(lockfd < 0)
    {
        perror("open error!");
        exit(-1);
    }

    // 对文件加共享锁
    int ret = flock(lockfd, LOCK_SH);
    if(ret < 0)
    {
        cout << "共享锁加锁失败！" << endl;
        perror("flock error!");
        exit(-1);
    }
    cout << "共享锁加锁成功！" << endl;
    
    for(int i = 0; i < 5; ++i)
    {
        cout << "aaaaaaaaaaaaaaaaa" << endl;
        sleep(1);
    }

    cout << "睡眠十秒种！" << endl;
    sleep(10);

    // 对文件加互斥锁
    ret = flock(lockfd, LOCK_EX);
    if(ret < 0)
    {
        cout << "互斥锁加锁失败！" << endl;
        perror("flock error!");
        exit(-1);
    }
    cout << "互斥锁加锁成功！" << endl;

    for(int i = 0; i < 5; ++i)
    {
        cout << "qqqqqqqqqqqqqqqqqqqqq" << endl;
        sleep(1);
    }

    // 解锁
    ret = flock(lockfd, LOCK_UN);
    if(ret < 0)
    {
        cout << "互斥锁解锁失败！" << endl;
        perror("flock error!");
        exit(-1);
    }
    cout << "互斥锁解锁成功！" << endl;

    return 0;
}
