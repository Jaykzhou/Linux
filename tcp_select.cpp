// 在单进程/单线程的情况下，利用 select 多路转接模型实现 TCP 服务器
// 多路转接可以利用内核进行监控，检测就绪事件，返回给用户态进行处理
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/select.h>
#include "tcp_socket.hpp" 
using namespace std;

// select 类
class Select 
{
  public:
    Select() :_maxfd(-1) {}

    // 初始化事件
    bool Init()
    {
      FD_ZERO(&_readset);
      return true;
    }

    // 向集合中添加事件
    bool Add(TcpSocket& sock)
    {
      int sockfd = sock.Getfd();
      FD_SET(sockfd, &_readset);

      _maxfd = sockfd > _maxfd ? sockfd : _maxfd;

      return true;
    }
    
    // 从集合中删除事件
    bool Delete(TcpSocket& sock)
    {
      int sockfd = sock.Getfd();
      FD_CLR(sockfd, &_readset);

      // 从后向前寻找最大的文件描述符
      for(int i = sockfd; i >= 0; ++i)
      {
        if(FD_ISSET(i, &_readset))
        {
          _maxfd = i;
          return true;
        }
      }

      _maxfd = -1;
      return true;
    }

    // 监控集合中的事件
    bool Wait(vector<TcpSocket>& lis, int sec = 3)
    {
      // select 将未就绪事件从集合中移除了，向用户态返回的为就绪事件集合。
      // 因此，只需要遍历整个集合，将所有就绪事件放到 vector 中即可。
      // 最后在将每个事件重新添加一次。
      fd_set temp_read = _readset;                       // 使用临时对象进行监控，不需要用户每次去添加事件
      struct timeval tv;
      tv.tv_sec = sec;
      tv.tv_usec = 0;
      int retnum = select(_maxfd + 1, &temp_read, NULL, NULL, &tv);
      if(retnum < 0)
      {
        cout << "select error" << endl;
        return false;
      }
      else if(retnum == 0)
      {
        cout << "wait timeout" << endl;
        return false;
      }
      
      // 返回就绪事件个数
      for(int i = 0; i <= _maxfd; ++i)
      {
        if(FD_ISSET(i, &temp_read))
        {
          TcpSocket clisock;
          clisock.Setsockfd(i);
          lis.push_back(clisock);
        }
      }

      return true;
    }

  private:
    fd_set _readset;              // 可读事件集合(其他事件没有用到，因此不进行监控)
    int _maxfd;                   // 最大文件描述符
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

  // Tcp服务器流程
  TcpSocket lissock;
  lissock.Socket();
  lissock.Bind(ip, port);
  lissock.Listen();

  // select 监听
  Select st;
  st.Init();
  st.Add(lissock);                           // 第一次进行监控，就绪的套接字只有一个监听套接字
  while(1)                
  {
    vector<TcpSocket> list;                  // list 是临时变量，因此一次循环以后它会析构，从而导致里面的socket也会析构。
    if(st.Wait(list) == false)               // 从而导致socket描述符关闭，导致监控时出现错误，因此socket析构函数中不能关闭描述符
    {                                        // 最后在我们不使用socket的时候再进行手动关闭。
      sleep(1);
      continue;
    }

    // 轮询处理list中所就绪的socket结构体
    for(auto sock : list)
    {
      if(sock.Getfd() == lissock.Getfd())
      {
        // 当前的套接字是监听套接字，因此它所做工作就是进行监听，用新的套接字获取已完成新连接
        TcpSocket clisock;
        if(lissock.Accept(clisock) == false)
        {
          continue;
        }

        st.Add(clisock);
      }
      else 
      {
        // 当前的套接字为普通套接字，因此它所做的工作就是处理数据
        string date;
        if(sock.Recv(date) == false)              // 处理数据出错时，直接丢弃该套接字，处理下一个套接字
        {
          st.Delete(sock);
          close(sock.Getfd());
          continue;
        }
        else 
        {
          cout << "Cli Say: " << date << endl;
        }
        
        date.clear();
        cin >> date;
        if(sock.Send(date) == false)
        {
          st.Delete(sock);
          close(sock.Getfd());
          continue;
        }
      }
    }
  }

  close(lissock.Getfd());
  
  return 0;
}
