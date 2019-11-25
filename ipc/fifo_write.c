// 命名管道的使用
// 命名管道可见于文件系统，会创建一个管道文件(文件只是名字)
// 管道通信的本质还是内核的那块缓冲区
// int mkfifo(const char *pathname, mode_t mode);
//        pathname : 管道文件的路径名
//        mode : 创建文件的权限
//  返回值： 0    失败：-1；

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
int main() {

  // 创建命名管道文件。
  char* file = "./tmp.fifo";
  umask(0);
  int ret = mkfifo(file,0664);
  if(ret < 0) {

    // 如果是其他错误则，直接退出。
    if(errno != EEXIST) {
    perror("mkfifo error");
    return -1;
    }
  }

  // 对命名管道文件进行操作:
  // open 返回文件描述符
  int fd = open(file,O_WRONLY);

  // 只读操作：
  while(1) {
    char buf[1024] = {0};
    scanf("%s",buf);
    write(fd,buf,strlen(buf));
  }
  
  close(fd);
  return 0;
}
