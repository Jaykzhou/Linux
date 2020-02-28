// 区别一下 fork 与 vfork 的区别
// 此列子测试 vfork 函数
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>
using namespace std;

int main()
{
  int date = 0;

  pid_t pid = 0;
  pid = vfork();
  if(pid < 0)
    perror("fork error:");
  else if(pid == 0)
  {
    date++;
    return 0;
  }
  else
    cout << "i am father" << endl;

  return 0;
}
