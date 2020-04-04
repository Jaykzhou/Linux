// 测试两个重定向函数 dup / dup2 
// 目标:
//   dup: 先打开一个文件，然后向文件中写入"i am a student!"，然后利用返回的文件描述符进行操作，继续写入"i feel very happy"。
//   dup2: 打开两个文件，然后进行重定向，使用两个文件描述符进行操作观察是否可以写入到一个文件中。 
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
using namespace std;

int main()
{
    // 测试 dup 函数
    int fd = open("./hello.txt", O_CREAT | O_RDWR | O_TRUNC, 0664);
    if(fd < 0) perror("open error!"), exit(-1);

    char* buf = "i am a student!";
    write(fd, buf, strlen(buf));

    int newfd = dup(fd);
    if(newfd < 0) perror("dup error!"), exit(-1);

    char* buf1 = "i feel very happy";
    write(newfd, buf1, strlen(buf1));

    char* buf2 = "hehe!";
    write(fd, buf2, strlen(buf2));

    close(fd);
    close(newfd);

    // 测试 dup2 函数
    int fd1 = open("./hehe.txt", O_CREAT | O_RDWR | O_TRUNC, 0664);
    if(fd1 < 0) perror("open error!"), exit(-1);
    int fd2 = open("./haha.txt", O_CREAT | O_RDWR | O_TRUNC, 0664);
    if(fd2 < 0) perror("open error"), exit(-1);

    char* buf3 = "hhhhhhhhhhhhhhh";
    char* buf4 = "aaaaaaaaaaaaaaa";

    write(fd2, buf3, strlen(buf3));    // 全缓冲，如果当前立刻在文件中读取数据时是无法读取数据的。
                                       // 只有 close 的时候才用刷新缓冲区，最后导致数据存在于文件。
    dup2(fd, fd2);

    write(fd2, buf4, strlen(buf4));

    close(fd1);

    return 0;
}
