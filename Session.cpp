// 查看进程ID，进程组ID，会话ID，体会接口的使用
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
  cout << "Father Process ID: " << getpid() << endl;
  pid_t pid = fork();
  if(pid < 0)
  {
    cerr << "fork error" << endl;
    return -1;
  }
  // 子进程
  else if(pid == 0)
  {
    cout << "Process ID: " << getpid() << endl;
    cout << "Group ID: " << getpgid(0) << endl;
    cout << "Session ID: " << getsid(0) << endl; 

    sleep(10);

    // 子进程不是组长进程，因此可以成为新会话的首进程(控制进程)，谁调用谁成为新会话首进程，成为组长进程。
    setsid();

    cout << "Changed:" << endl;
    cout << "Process ID: " << getpid() << endl;
    cout << "Group ID: " << getpgid(0) << endl;
    cout << "Session ID: " << getsid(0) << endl; 
    sleep(20);
  }

  return 0;
}
