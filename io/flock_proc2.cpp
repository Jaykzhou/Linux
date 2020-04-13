// 文件锁兼容性问题
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

#define FILENAME "./file.txt"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cout << "./a.out 1/2 (1:共享锁，2:互斥锁)" << endl;
        return -1;
    }

    int lockfd = open(FILENAME, O_RDWR);
    if(lockfd < 0)
    {
        perror("open error!");
        exit(-1);
    }

    // 文件锁(1:共享锁 / 2:互斥锁)
    int mainnum = atoi(argv[1]);
    int opt = 0;
    if(mainnum == 1)
        opt = LOCK_SH;
    else 
        opt = LOCK_EX;
    int ret = flock(lockfd, opt);
    if(ret < 0)
    {
        cout << "加锁失败！" << endl;
        perror("flock error!");
        exit(-1);
    }
    cout << "加锁成功！" << endl;

    // 加锁成功
    for(int i = 0; i < 5; ++i)
    {
        cout << "aaaaaaaaaaaaaa" << endl;
        sleep(1);
    }

    // 解锁
    ret = flock(lockfd, LOCK_UN);
    if(ret < 0)
    {
        cout << "解锁失败！" << endl;
        perror("flock error!");
        exit(-1);
    }
    if(mainnum == 1)
        cout << "共享锁解锁成功！" << endl;
    else 
        cout << "互斥锁解锁成功！" << endl;

    return 0;
}
