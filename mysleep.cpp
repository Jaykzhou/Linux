// 利用闹钟模拟实现sleep函数
#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

void handle(int signo)
{
  cout << "睡眠状态已解除！" << endl;
}

void _sleep(int s)
{
  // 定义 SIGLARM 信号的处理方式(默认终止进程)
  signal(SIGALRM, handle);
  
  cout << "进入睡眠状态！" << endl;
  alarm(s);
  pause();
}


int main()
{
  while(1)
  {
    cout << "hello world!" << endl;
    _sleep(1);
  }

  return 0;
}
