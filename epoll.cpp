// 利用epoll来监控标准输入，从而体会多路转接epoll模型
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
using namespace std;

// 异步阻塞操作
// int epoll_create(int size);        在内核中创建eventepoll结构, 返回该结构操作句柄
// int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
// int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);  

void SetNonblock()
{
  int fd = fcntl(0, F_GETFL); 
  fcntl(0, F_SETFL, fd | O_NONBLOCK);
}

int main()
{
  // 设置非阻塞操作                   当没有就绪事件时，做其他事情。当有就绪事件时，处理事件。
  SetNonblock();

  int epfd = epoll_create(1024);
  if(epfd < 0)
  {
    cout << "create error" << endl;
    return -1;
  }

  struct epoll_event event;
  event.events = EPOLLOUT;
  event.data.fd = 0;
  epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);

  // 开始监听
  while(1)
  {
    cout << "正在做其他任务~~~~" << endl;
    sleep(1);

    int num = epoll_wait(epfd, &event, 1, 3000);
    if(num < 0)
    {
      cout << "epoll_wait error" << endl;
      return -1;
    }
    else if(num == 0)
    {
      cout << "wait timeout" << endl;
      return -1;
    }

    // 返回就绪的文件描述符
    int fd = event.data.fd;
    char buf[1024] = {0};
    read(fd, buf, 1023);
    cout << buf;
  }

  return 0;
}
