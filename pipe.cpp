// 管道 -> 单向通信
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
using namespace std;

void sigcd(int signo)
{
  cout << "信号:";
  cout << signo << endl;

}

int main()
{
  // 重定义信号处理方式
  signal(SIGPIPE, sigcd);

  int fd[2] = {0};
  int ret = pipe(fd);
  if(ret < 0)
    cout << "pipe error" << endl;

  pid_t pid = 0;
  pid = fork();
  if(pid < 0)
    cout << "fork error" << endl;
  else if(pid > 0)
  {
    // 父进程
    close(fd[0]);
    while(1)
    {
      char buf[1024] = {'\0'};
      cin >> buf;
      write(fd[1], buf, strlen(buf));
      //close(fd[1]);                                // 所有读端关闭，写端读出数据后退出
      //sleep(10);
    }
  }
  else 
  {
    // 子进程
    close(fd[1]);
    while(1)
    {
      char buf[1024] = {'\0'};
      read(fd[0], buf, 1024);
      cout << buf << endl;
      close(fd[0]);
      sleep(1);
    }
  }

  return 0;
}
