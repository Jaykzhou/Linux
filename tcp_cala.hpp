// 传输层基于 TCP 协议的网络版计算器
// 此文件中主要实现网络相关的部分以及计算过程中所需要的共用数据
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

// 数据处理：
// 1. 利用结构体实现两位数的运算
// 2. 利用字符串实现不定位数的处理(我们选择它)


class Tcp_socket
{
  public:
    Tcp_socket() :_sockefd(-1) {}

    ~Tcp_socket()
    {
      if(_sockefd < 0)
      {
        close(_sockefd);
        _sockefd = -1;
      }
    }

    // 1. 创建套接字
    bool Socket()
    {
      _sockefd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if(_sockefd < 0)
      {
        cerr << "socket error" << endl;
        return false;
      }

      return true;
    }

    // 2. 绑定地址信息(注: 客户端不需要进行绑定, 操作系统自行进行绑定)
    bool Bind(string& ip, uint16_t port)
    {
      struct sockaddr_in sockaddr;
      sockaddr.sin_family = AF_INET;
      sockaddr.sin_addr.s_addr  = inet_addr(ip.c_str());
      sockaddr.sin_port = htons(port);
      socklen_t len = sizeof(sockaddr);

      int ret = bind(_sockefd, (struct sockaddr*)&sockaddr, len);
      if(ret < 0)
      {
        cerr << "bind error" << endl;
        return false;
      }

      return true;
    }

    // 3. 服务器监听客户端请求
    bool Listen(const int blogsize = 10)
    {
      int ret = listen(_sockefd, blogsize);
      if(ret < 0)
      {
        cerr << "listen error" << endl;
        return false;
      }

      return true;
    }

    // 3. 客户端向服务器发起连接请求
    bool Connect(const string& ip, uint16_t port)
    {
      struct sockaddr_in sockaddr;
      sockaddr.sin_family = AF_INET;
      sockaddr.sin_addr.s_addr  = inet_addr(ip.c_str());
      sockaddr.sin_port = htons(port);
      socklen_t len = sizeof(sockaddr);

      int ret = connect(_sockefd, (struct sockaddr*)&sockaddr, len);
      if(ret < 0)
      {
        cerr << "conncet error" << endl;
        return false;
      }

      return true;
    }

    void Setsockfd(int newfd)
    {
      _sockefd = newfd;
    }

    // 4. 获取已完成新连接
    bool Accept(Tcp_socket& newsock)
    {
      // 获取新连接的地址信息(当想要获取时，通过函数参数传出即可)
      struct sockaddr_in sockaddr;
      socklen_t len = sizeof(sockaddr);
      int newfd = accept(_sockefd, (struct sockaddr*)&sockaddr, &len);
      if(newfd < 0)
      {
        cerr << "accept error" << endl;
        return false;
      }

      newsock.Setsockfd(newfd);
      return true;
    }

    // TCP 保证可靠传输
    bool Send(string& numstr)
    {
      int ret = send(_sockefd, numstr.c_str(), numstr.size(), 0);
      if(ret < 0)
      {
        cerr << "ret error" << endl;
        return false;
      }

      return true;
    }

    bool Send(int num)
    {
      // char* itoa(int value, char * str, int base);  C 标准库，Linux平台不支持 stdlib.h
      // base: 进制标识(2, 8, 10, 16);

      char result[1024] = {0};

      int len = sprintf(result, "%d", num);        //  C 标准库，Linux平台支持   stdio.h  
      
      int ret = send(_sockefd, result, len, 0);
      if(ret < 0)
      {
        cerr << "ret error" << endl;
        return false;
      }

      return true;
    }

    bool Recv(string& numstr)
    {
      char temp[1024] = {0};
      int retval = recv(_sockefd, &temp, 1023, 0);
      if(retval == 0)
      {
        cout << "peer shutdown" << endl;
        return false;
      }
      else if(retval < 0)
      {
        cerr << "recv error" << endl;
        return false;
      }

      numstr.assign(temp, retval);
      return true;
    }

  private:
    int _sockefd;
};
