// 利用多路转接 epoll 模型实现 Tcp 服务器
#include <iostream>
#include <sys/epoll.h>
#include <vector>
#include "tcp_socket.hpp"
using namespace std; 

#define NODESIZE 1024
#define LISTNODE 100

// Epoll 类
class Epoll 
{
  public:
    // 内核中创建eventpoll结构体
    bool Init()
    {
      _epfd = epoll_create(NODESIZE);
      if(_epfd < 0)
      {
        cout << "create error" << endl;
        return false;
      }

      return true;
    }

    // 添加用户关心socket事件
    bool Add(TcpSocket& sock)
    {
      struct epoll_event event;
      event.events = EPOLLIN | EPOLLOUT;            // 对于一个socket结构体来说我们即关心读事件又关心写事件。 
      event.data.fd = sock.Getfd();
      // 为描述符添加一个epoll_event事件
      int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, sock.Getfd(), &event);   // 向红黑树中添加一个结点(key = Value)
      if(ret < 0)
      {
        cout << "create error" << endl;
        return false;
      }

      return true;
    }

    // 删除某一个事件
    bool Del(TcpSocket& sock)
    {
      // 用来获取当前删除事件的信息(当不想获取时，直接设置为空)
      struct epoll_event event;
      int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, sock.Getfd(), &event);
      if(ret < 0)
      {
        cout << "delete error" << endl;
        return false;
      }

      return true;
    }

    // 进行事件监控
    bool Wait(vector<TcpSocket>& list, int sec = 3000)
    {
      struct epoll_event event[LISTNODE];
      int ret = epoll_wait(_epfd, event, LISTNODE, sec);
      if(ret < 0)
      {
        cout << "epoll_wait error" << endl;
        return false;
      }
      else if(ret == 0)
      {
        cout << "wait timeout" << endl;
        return false;
      }

      // 将就绪的事件添加到所给的vector中
      for(int i = 0; i < ret; ++i)
      {
        TcpSocket socket;
        socket.Setsockfd(event[i].data.fd);
        list.push_back(socket);
      }

      return true;
    }

  private:
    int _epfd;
};


int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.249.129 9000" << endl;
    return -1;
  }

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  TcpSocket lissocket;
  lissocket.Socket();
  lissocket.Bind(ip, port);
  lissocket.Listen();

  Epoll epoll;
  epoll.Init();
  epoll.Add(lissocket);
  while(1)
  {
    // 利用epoll监听就绪事件
    vector<TcpSocket> list;        // list 是临时对象，每进行一次循环就会被析构，从而导致我们的socket对象也会析构。
    epoll.Wait(list);              // 但是由于我们还需继续通信，因此，不能在socket类析构函数中关闭文件描述符。

    for(auto sock : list)
    {
      if(sock.Getfd() == lissocket.Getfd())
      {
        // 当前的socket为监听socket
        TcpSocket clisocket; 
        if(sock.Accept(clisocket) == false)
        {
          epoll.Del(sock);
          continue;
        }
        epoll.Add(clisocket);
      }
      else 
      {
        // 当前的socket为工作socket
        string data;
        if(sock.Recv(data) == false)
        {
          epoll.Del(sock);
          continue;
        }
        cout << "Cli say:" << data << endl;
        data.clear();
        cin >> data; 
        if(sock.Send(data) == false)
        {
          epoll.Del(sock);
          continue;
        }
      }
    }
  }

  return 0;
}
