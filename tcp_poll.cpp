// 多路转接 poll 实现 Tcp 服务端
#include <iostream>
#include <string>
#include <vector>
#include <sys/poll.h>
#include <assert.h>
#include "tcp_socket.hpp"
using namespace std;

#define SIZE 10 

// 封装 poll 类
class Poll
{
  public:
    bool Init(int size = SIZE)
    {
      // 开空间
      _event = (pollfd*)malloc(sizeof(pollfd) * size);
      assert(_event);
      for(int i = 0; i < size; ++i)
      {
        _event[_size].fd = -1;
        _event[_size].events = 0;
      }

      return true;
    }

    // 添加事件(默认可读事件)
    bool Add(TcpSocket& sock)
    {
      _event[_size].fd = sock.Getfd();
      _event[_size].events = POLLIN;

      _size++;
 
      return true;
    }

    // 删除事件
    bool Delete(TcpSocket& sock)
    {
      for(int i = 0; i < _size; ++i)
      {
        if(_event[i].fd == sock.Getfd())
        {
          _event[i].events = 0;                        // 如果在多个事件中只删除一个事件的话，那么只要将该事件的比特位置为 0
          _event[i].fd = -1;                           // -1 表示丢掉当前的文件描述符
        }
      }

      return true;
    }

    // 监听事件
    bool Wait(vector<TcpSocket>& event, int time = 3000)
    {

      pollfd* thing = _event;                        // 利用临时对象，每次就不需要重新添加事件了。

      int ret = poll(thing, _size, time);
      if(ret < 0)
      {
        cout << "poll error" << endl;
        return false;
      }
      else if(ret == 0)
      {
        cout << "wait timeout" << endl;
        return false;
      }
      
      for(int i = 0; i < _size; ++i)                 // 遍历整个事件集合数组，观察哪一个事件准备就绪，从而对就绪进行处理。
      {
        if(thing[i].fd != -1)
        {
          if(thing[i].revents & POLLIN)
          { 
            // 可读事件就绪
            TcpSocket sock;
            sock.Setsockfd(thing[i].fd);
            event.push_back(sock);
          }
        }
      }

      return true;
    }

    // 清空所用资源
    void Clear()
    {
      free(_event);
    }

  private:
    pollfd* _event;
    int _size = 0;
};

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out ip port" << endl;
    return -1;
  }

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  // 创建监听套接字
  TcpSocket lissock;
  lissock.Socket();
  lissock.Bind(ip, port);
  lissock.Listen();

  Poll po;
  po.Init();
  po.Add(lissock);

  while(1)
  {
    // poll 进行监听
    vector<TcpSocket> event;
    po.Wait(event);

    // 处理已就绪套接字
    for(auto& sock : event)
    {
      if(sock.Getfd() == lissock.Getfd())
      {
        // 监听套接字就绪
        TcpSocket clisock;
        sock.Accept(clisock);
        po.Add(clisock);
      }
      else 
      {
        // 客户端套接字
        string buf;
        sock.Recv(buf);
        cout << "Cli Say :" << buf << endl;

        buf.clear();
        cin >> buf;
        sock.Send(buf);
      }
    }
  }

  po.Clear();

  return 0;
}
