// 利用 poll 多路转接模型监听标准输入
#include <iostream>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

// 非阻塞IO模式
void SETNONBLOCK()
{
  int flags =  fcntl(0, F_GETFL);
  fcntl(0,flags | F_SETFL);
}

// int poll(struct pollfd *fds, nfds_t nfds, int timeout);  timeout 的单位为毫秒
int main()
{
  // 创建 pollfd 结构体，添加关心的文件描述符
  struct pollfd ev;
  ev.fd = 0;
  ev.events = POLLIN;

  while(1)
  {
    // 进行监听
    int time = 0;                                     // 将 poll 函数设置为 非阻塞模式。
    int ret = poll(&ev, 1, time);
    if(ret < 0)
    {
      cout << "poll error" << endl;
      return -1;
    }
    else if(ret == 0)
    {
      cout << "wait timeout" << endl;
      sleep(1);
      continue;
    }

    // 依次处理所发生的事件
    for(int i = 0; i < ret; ++i)
    {
      if(ev.revents & POLLIN)                         // 对应比特位进行 "与" 操作
      {
        // 读取标准输入中的数据(程序启动默认打开0，1，2文件描述符)
        char buf[1024] = {0};
        read(0, buf, 1024);                           // 在进行IO的还是阻塞的，由于数据量太小，数据从驱动到缓冲区时间太短。
        cout << "stdin: " << buf << endl;             // 为了高效，我们可以将 IO 设置为非阻塞模型。
      }
    }


  }

  return 0;
}
