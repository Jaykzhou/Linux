// 利用守护进程书写一个服务程序，服务程序的作用：
// 每隔 10s 向 mywork.txt 写入当前时间
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
using namespace std;

bool Demon()
{
  pid_t pid = fork();
  if(pid < 0)
  {
    cerr << "fork error" << endl;
    return false;
  }
  else if(pid > 0)
  {
    exit(0);
  }

  setsid();                                         // 成功返回 新会话 ID.

  chdir(".");                                       // 成功返回 0.

  umask(0002);

  close(0);
  open("/dev/null", O_RDWR);
  dup2(1, 0);
  dup2(2, 0);

  return true;
}

void Write_Text(const int fd)
{
  // 获取当前系统时间
  time_t t;
  char buf[1024] = {0};
  time(&t);
  ctime_r(&t, buf);

  // 向文件中写入当前系统时间
  write(fd, buf, 29);
}

int main()
{
  Demon();
  
  // 创建新的文件
  int fd = open("mywork.txt", O_CREAT | O_RDWR);      // 返回文件描述符表
  while(1)
  {
    Write_Text(fd);
    sleep(10);
  }

  return 0;
}
