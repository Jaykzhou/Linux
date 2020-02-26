// 僵尸进程的产生，处理
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
using namespace std;

// SIGCHLD 信号的处理方式
void sigcd(int data)
{
  int num = 0;
  waitpid(17, &num, WNOHANG);
  cout << "我收到了 SIGCHLD 信号了~~" << endl;

  cout << num << endl;
}

int childfun()
{
  int num = 1024;
  cout << "我在学习中~~~" << endl;
  exit(num);
}

void fatherfun()
{
  cout << "我在打电话~~~" << endl;
}

int main()
{
  // 重定义 SIGCHLD 信号的处理方式
  signal(SIGCHLD, sigcd);

  pid_t pid = 0;
  pid = fork();
  if(pid == 0)
  {
    // 子进程
    childfun();
  }
  else
  {
    // 父进程
    fatherfun();
    sleep(20);
  }

  return 0;
}
