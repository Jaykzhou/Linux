// 管道 -> 双向通信
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
using namespace std;

int main()
{
  int fdin[2] = {0};             // 父 -> 子
  int fdout[2] = {0};            // 子 -> 父
  int ret1 = pipe(fdin);
  if(ret1 < 0)
    cout << "pipe error" << endl;
  int ret2 = pipe(fdout);
  if(ret2 < 0)
    cout << "pipe error" << endl;

  pid_t pid = 0;
  pid = fork();
  if(pid < 0)
    cout << "fork error" << endl;
  else if(pid > 0)
  {
    // 父进程
    while(1)
    {
      close(fdin[0]);
      char data[] = "hello!";
      write(fdin[1], data, strlen(data));

      close(fdout[1]);
      char buf[1024] = {0};
      read(fdout[0], buf, 1024);
      cout << "father:" << buf << endl;
      sleep(1);
    }
  }
  else 
  {
    // 子进程
    while(1)
    {
      close(fdout[0]);
      char data[] = "world!";
      write(fdout[1], data, strlen(data));

      close(fdin[1]);
      char buf[1024] = {0};
      read(fdin[0], buf, 1024);
      cout << "child:" << buf << endl;
      sleep(1);
    }
  }

  return 0;
}
