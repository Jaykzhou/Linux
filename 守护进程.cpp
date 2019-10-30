// 守护进程的编写流程:
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int main()
{
  // 创建子进程，父进程退出
  pid_t pid = fork();
  if(pid < 0)
  {
    cerr << "fork error" << endl;
    return -1;
  }
  else if(pid != 0)                         // 父进程直接退出
  {
    return 0;
  }

  // 子进程中建立新的会话
  setsid();

  // 改变当前工作目录
  // int chdir(const char *path);           将当前工作路径改变为指定的工作路径
  int ret = chdir("/home/superao/testgit/GitHub/Linux");
  if(ret < 0)
  {
    cerr << "chdir error" << endl;
  }

  // 设置文件掩码
  // mode_t umask(mode_t mask);             设置文件权限创建掩码(777 - 所需属性 = 文件掩码)
  umask(000);
  
  // 关闭继承下来的文件描述符  
  // POSIX标准接口函数: 1. close(0), close(1), close(2)
  close(0);
  open("/dev/null", O_RDWR);
  dup2(1, 0);
  dup2(2, 0);


  int fd = open("./a.txt", O_CREAT | O_RDWR);
  while(1)
  {
    // 守护进程的核心任务
    string date = "hello world!";
    write(fd, date.c_str(), date.size());
  }
  
  return 0;
}
