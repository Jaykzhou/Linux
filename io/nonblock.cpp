// 以非阻塞的模式从标准输入读取数据，将读到的数据进行打印
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
using namespace std;

// 启用非阻塞模式
void SetNonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

int main()
{
    SetNonblock(0);             // 标准输入设置为非阻塞

    char buf[1024];
    while(1)
    {
        bzero(buf, 1024);
        int ret = read(0, buf, 1024);
        if(ret < 0)
        {
            if(errno == EAGAIN)
            {
                cout << "当前没有数据可读！" << endl;
            }
        }
        else 
        {
            cout << "读取的数据为: " << buf;
        }

        cout << "处理其他任务！" << endl;
        sleep(1);
    }

    return 0;
}
