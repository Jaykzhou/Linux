// 利用select模型监听标准输入(0号描述符)，体会多路转接IO的功能
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
using namespace std;

// int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
void Select()
{
  // 最大文件描述符
  int maxfd = 0;

  // 可读事件集合
  fd_set readset;
  // 清空可读事件集合
  FD_ZERO(&readset);

  // 将标准输入添加到可读事件集合中
  FD_SET(0, &readset);
  
  // 加载到内核中进行监控
  while(1)
  {
    // 体会阻塞与非阻塞监听
    cout << "i am student." << endl;
    sleep(2);

    // 限时阻塞的时间限制(注: 内核中的限时阻塞时间需要每检测一次设置一次，内核在监听完毕之后会自动的将其置为 0)
    struct timeval tv;                                      // 将阻塞时间设置为 0，表示当前为非阻塞模式。
    tv.tv_sec = 0;                                          // 秒
    tv.tv_usec = 0;                                         // 毫秒
    
    FD_SET(0, &readset);                                    // 每次向集合中重新添加事件，放到内核中进行监控
    int nfd = select(maxfd + 1, &readset, NULL, NULL, &tv);
    if(nfd < 0)
    {
      cout << "select error" << endl;
      continue;
    }
    else if(nfd == 0)
    {
      cout << "wait timeout" << endl;
      continue;
    }

    // 内核监控到可读事件，并且返回了可读事件集合
    for(int i = 0; i <= maxfd; ++i)
    {
      // 读取数据
      char buf[1024] = {0};
      read(0, buf, 1023);
      cout << "stdin: " << buf << endl;

      // 将处理完的事件从集合中去除
      FD_CLR(i, &readset);
    }
  }
}

int main()
{
  Select();
  return 0;
}

