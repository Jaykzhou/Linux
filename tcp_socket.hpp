// 传输层基于 TCP 协议的客户端/服务端程序框架
// 封装 TCP 流程:
//    1. 创建套接字
//    2. 绑定地址信息
//    3. 开始监听(服务端) / 发送链接请求(客服端)
//    4. 获取已完成新链接(服务端)    三次握手在 Accept 之前完成
//    5. 发送数据
//    6. 接受数据
//    7. 关闭套接字
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CHECK(p) if(p == false) {return -1;}

using namespace std;

class TcpSocket
{
  public:
    TcpSocket() :_sockfd(-1) {}
    ~TcpSocket() 
    {
      if(_sockfd >= 0)   // 标准输入有可能被关闭，由最小分配原则可知有可能将 0 号文件描述符进行分配。
      {
        close(_sockfd);
        _sockfd = -1;
      }
    }

    bool Socket()
    {
      _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if(_sockfd < 0)
      {
        cerr << "socket error" << endl;
        return false;
      }
    
      return true;
    }

    bool Bind(string& ip, uint16_t port)
    {
      // 创建地址结构
      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = inet_addr(ip.c_str());
      socklen_t len = sizeof(addr);

      // 绑定地址
      int ret = bind(_sockfd, (struct sockaddr*)&addr, len);
      if(ret < 0)
      {
        cerr << "bind error" << endl;
        return false;
      }

      return true;
    }

    bool Listen(int backlog = 10)
    {
      // 服务端开始监听, 获取链接请求
      int ret = listen(_sockfd, backlog);
      if(ret < 0)
      {
        cerr << "listen error" << endl;
        return false;
      }

      return true;
    }

    bool Connect(string& ip, uint16_t port)
    {
      // 客户端向服务端发送链接请求
      struct sockaddr_in srvaddr;
      srvaddr.sin_family = AF_INET;
      srvaddr.sin_port = htons(port);
      srvaddr.sin_addr.s_addr = inet_addr(ip.c_str());
      socklen_t len = sizeof(srvaddr);

      int ret = connect(_sockfd, (struct sockaddr*)& srvaddr, len);
      if(ret < 0)
      {
        cerr << "connect error" << endl;
        return false;
      }

      return true;
    }

    bool Accept(TcpSocket& newsock)
    {
      // 已完成链接的socket描述符
      int newfd = -1;

      // 客户端地址信息
      struct sockaddr_in cliaddr;
      socklen_t len = sizeof(cliaddr);

      newfd = accept(_sockfd, (struct sockaddr*)&cliaddr, &len);
      if(newfd < 0)
      {
        cerr << "accept error" << endl;
        return false;
      }

      newsock.Setsockfd(newfd);
      return true;
    }

    bool Send(string& date)
    {
      int ret = send(_sockfd, date.c_str(), date.size(), 0);
      if(ret < 0)
      {
        cerr << "send error" << endl;
        return  false;
      }

      return true;
    }

    bool Recv(string& date)
    {
      char temp[1024] = {0};
      int ret = recv(_sockfd, temp, 1023, 0);     // 当没有数据的时候，它会阻塞，等待数据，而不是返回 0 
      if(ret == 0)
      {
        cout << "peer shutdown" << endl;
        return false;
      }
      else if(ret < 0)
      {
        cerr << "recv error" << endl;
        return false;
      }

      date.assign(temp, ret);
      return true;
    }

    void Setsockfd(int fd)
    {
      _sockfd = fd;
    }

  private:
    int _sockfd;
};
