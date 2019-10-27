// 测试孤儿进程，体会孤儿进程的现象。
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
  // 创建子进程
  pid_t pid = fork();
  if(pid < 0 )
  {
    cout << "fork error" << endl;
    return -1;
  }
  else if(pid == 0)
  {
    // 子进程
    while(1)
    {
      cout << "Hello World!" << endl;
    }
  }

  sleep(100);
  cout << "Father exit" << endl;
  return 0;
}
// 总结:
// 1. 起始进程的父进程是终端 bash(shell 的一种) ，它受终端的控制。
// 2. 孤儿进程的父进程是 init 进程，表面上不受终端控制，但是当我们使用 ps -ajx 查看时它还是守终端控制。
// 3. 程序中的信号是发送给整个进程组的，因此上，有些信号发生之后，整个进程组都需要退出，而有些信号是自定义进行处理。 
