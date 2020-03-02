// 比较 exit 与 _exit 的区别:
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

void childfun()
{
  cout << "hhhhhhhhhhhhhhh";
  // return                   只会退出当前函数
  // exit(0);                    // 退出当前整个进程
  _exit(0);                      // 退出当前整个进程，并刷新文件缓冲区
}
int main()
{
  pid_t pid = 0;
  pid = fork();
  if(pid < 0)
    perror("fork error:");
  else if(pid > 0)
  {
    // 父进程
    sleep(20);
  }
  else 
  {
    // 子进程
    childfun();
    cout << "子进程函数出来了" << endl;
  }

  return 0;
}
