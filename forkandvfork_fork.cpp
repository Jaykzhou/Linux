// 区别一下 fork 与 vfork 的区别
// 此列子测试 fork 函数
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>
using namespace std;

void childfun()
{
  int i = 20;
  while(i--) cout << "i am child!" << endl, sleep(1);
  return;
}

void fatherfun()
{
  while(waitpid(SIGCHLD, NULL, WNOHANG) > 0) sleep(3);
  while(1) cout << "i am father!" << endl, sleep(1);
}

int main()
{
  pid_t pid = 0;
  pid = fork();
  if(pid < 0)
    perror("fork error:");
  else if(pid == 0)
    childfun();
  else 
    fatherfun();

  return 0;
}
