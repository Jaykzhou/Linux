// 创建临时文件，体会视频缓存文件的工作原理
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
using namespace std;

int main()
{
    // 创建文件
    int fd = open("./file.txt", O_CREAT | O_RDWR, 0664);
    if(fd < 0) perror("open error!"), exit(-1);

    // 删除文件(此时并不立刻删除，等当前进程结束后，OS会自动释放)
    unlink("./file.txt");

    // 相关操作(file.txt -> mydata.txt)
    char* buf = "视频缓存数据!";
    int ret = write(fd, buf, strlen(buf));
    if(ret < 0) perror("write error!"), exit(-1);

    // 创建普通文件，从临时文件中读取数据
    int ffd = open("./mydata.txt", O_CREAT | O_RDWR, 0664);
    if(ffd < 0) perror("open error!"), exit(-1);

    // 读取数据，打印数据
    lseek(fd, 0, SEEK_SET);
    char data[1024];
    bzero(data, 1024);
    read(fd, data, 1024);
    write(ffd, data, 1024);

    lseek(ffd, 0, SEEK_SET);
    bzero(data, 1024);
    read(ffd, data, 1024);

    cout << data << endl;

    close(ffd);

    return 0;
}
