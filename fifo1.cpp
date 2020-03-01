// 命名管道 -> 双向
// 向另一个文件发送数据，为发送与接受不相互影响效率使用并发操纵
// 子进程：管道1 来写数据
// 父进程：管道2 来读数据
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
using namespace std;

#define FILENAME "./fifo"
#define FILENAME1 "./fifo1"

int main()
{
  // 1. 创建命名管道
  int ret = mkfifo(FILENAME, 0664);
  if(ret < 0)
      perror("mkfifo error");
  int ret1 = mkfifo(FILENAME1, 0664);
  if(ret1 < 0)
      perror("mkfifo error");

  // 2. 打开命名管道(可读可写)
  int fd = open(FILENAME, O_RDWR);
  if(fd < 0)
    perror("open error");
  int fd1 = open(FILENAME1, O_RDWR);
  if(fd1 < 0)
    perror("open error");
  
  // 3. 创建进程后进程通信
  pid_t pid = 0;
  pid = fork();
  if(pid < 0)
    perror("fork error");
  else if(pid > 0)
  {
    // 父进程
    while(1)
    {
      char buf[1024] = {0};
      read(fd1, buf, 1024);
      cout << buf << endl;
    }
  }
  else 
  {
    // 子进程
    while(1)
    {
      char buf[] = "i have a dream!";
      write(fd, buf, strlen(buf));
      sleep(1);
    }
  }
  
  return 0;
}
