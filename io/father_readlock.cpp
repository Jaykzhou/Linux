// 测试写锁与读锁的优先级
// 思路:
//  1. 父进程先获得读锁
//  2. 子进程1先申请写锁进行等待
//  3. 子进程2再申请读锁
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
using namespace std;

#define FILENAME "./file.txt"
#define MODE 0664

// 加锁
void mylock(int fd, int type)
{
    struct flock flock;
    flock.l_type = type;
    flock.l_whence = SEEK_SET;
    flock.l_start = 0;
    flock.l_len = 0;

    int ret = fcntl(fd, F_SETLKW, &flock);
    if(ret < 0)
    {
        cout << "加锁失败！" << endl;
        exit(-1);
    }
}

// 解锁
void myunlock(int fd)
{
    struct flock flock;
    flock.l_type = F_UNLCK;
    flock.l_whence = SEEK_SET;
    flock.l_start = 0;
    flock.l_len = 0;

    int ret = fcntl(fd, F_SETLK, &flock);
    if(ret < 0)
    {
        cout << "加锁失败！" << endl;
        exit(-1);
    }
}

int main()
{
    int fd = open(FILENAME, O_CREAT | O_RDWR, MODE);
    if(fd < 0)
    {
        perror("open error!");
        exit(-1);
    }

    cout << "父进程尝试加读锁！" << endl;
    mylock(fd, F_RDLCK);
    cout << "父进程加读锁成功！" << endl;

    if(fork() == 0)
    {
        // 子进程 1
        sleep(1);
        cout << "子进程1尝试加写锁！" << endl;
        mylock(fd, F_WRLCK);
        cout << "子进程1加写锁成功！" << endl;

        sleep(3);
        myunlock(fd);
        cout << "子进程1释放了写锁！" << endl;
        return 0;
    }

    if(fork() == 0)
    {
        // 子进程 2
        sleep(3);
        cout << "子进程2尝试加读锁！" << endl;
        mylock(fd, F_RDLCK);
        cout << "子进程2加读锁成功！" << endl;

        myunlock(fd);
        cout << "子进程2释放了读锁！" << endl;
        return 0;
    }

    sleep(5);          // 暂时先不释放锁，观察谁会先拿到锁。
    myunlock(fd);
    cout << "父进程释放读锁！" << endl;
    
    return 0;
}
