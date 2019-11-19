// 利用监听标准输入来体会水平触发与边缘触发的工作方式
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
using namespace std;

#define MAXSIZE 10000

void Nonblock()
{
  int flag = fcntl(0, F_GETFL);
  fcntl(0, F_SETFL , flag | O_NONBLOCK);
}

int main()
{

  Nonblock();

  int epfd = epoll_create(MAXSIZE);
  if(epfd < 0)
  {
    cout << "create error" << endl;
    return -1;
  }

  struct epoll_event event;
  event.events = EPOLLIN | EPOLLET;                                        // 默认水平触发
  event.data.fd = 0;
  int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);
  if(ret < 0)
  {
    cout << "crl error" << endl;
    return -1;
  }

  while(1)
  {
    ret = epoll_wait(epfd, &event, 1, 3000);
    if(ret < 0)
    {
      cout << "wait error" << endl;
      return -1;
    }
    else if(ret == 0)
    {
      cout << "wait timeout" << endl;
      continue;
    }

    // 初始就绪事件
    char buf[1024] = {0};
    // read(0, buf, 5);                                                     // 通常在边缘触发模式下，最好使用使用循环读取数据  
    while(1)                                                                // 并且需要将 IO 模式设置为非阻塞，否则会一直阻塞读取
    {
      ret = read(0, buf, 5);
      if(ret <= 0)
      {
        if(errno == EAGAIN)
        {
          break;
        }
      }
      
      cout << "Say :" << buf << endl;
    }

  }

  return 0;
}
