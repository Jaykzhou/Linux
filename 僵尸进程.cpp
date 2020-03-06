// 僵尸进程的产生，处理
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

// SIGCHLD 信号的处理方式
void sigcd(int signo)
{
  int num;
  int ret = waitpid(-1, &num, 0);
  if(ret < 0)
      perror("waitpid error");

  if(WIFEXITED(num))
  {
    num = WEXITSTATUS(num);
    cout << "返回状态码: " << num << endl;
  }
}

void childfun()
{
  cout << "我在学习中~~~" << endl;
  exit(-1);
}

void fatherfun()
{
  cout << "我在打电话~~~" << endl;
  while(1);
}

int main()
{
  signal(SIGCHLD, sigcd);

  pid_t pid = 0;
  pid = fork();
  if(pid < 0)
      perror("fork error");
  else if(pid == 0)
    childfun();
  else
    fatherfun();

  return 0;
}
