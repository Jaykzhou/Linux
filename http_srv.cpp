// 实现HTTP服务器: 不管客户端发送什么请求，服务端都响应 hello world！
// http协议传输层基于Tcp协议所实现的。
// 流程: 
//    1. 创建套接字
//    2. 绑定地址信息
//    3. 开始监听(服务端)
//    4. 获取已完成新连接
//    5. 接受数据(打印客户端发送的请求信息)
//    6. 发送数据(数据处理符合HTTP协议格式)
//    7. 关闭套接字
// 将套接字流程封装为一个类进行实现
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <pthread.h>
using namespace std;

#define CHECK_RET(p) if(p == false) {return -1;}

class Tcp_Scoket
{
  public:
    Tcp_Scoket() :_sockfd(-1) {}
    ~Tcp_Scoket() 
    {
      if(_sockfd >= 0)
      {
        close(_sockfd);
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

    bool Bind(const string& ip, const uint16_t port)
    {
      struct sockaddr_in srvaddr;
      srvaddr.sin_family = AF_INET;
      srvaddr.sin_addr.s_addr = inet_addr(ip.c_str());
      srvaddr.sin_port = htons(port);
      socklen_t len = sizeof(srvaddr);

      int ret = bind(_sockfd, (struct sockaddr*)&srvaddr, len);
      if(ret < 0)
      {
        cerr << "bind error" << endl;
        return false;
      }
      return true;
    }

    // 最大并发数
    bool Listen(int backlog = 10)
    {
      int ret = listen(_sockfd, backlog);
      if(ret < 0)
      {
        cerr << "listen error" << endl;
        return false;
      }
      return true;
    }

    void Setsockfd(int newsock)
    {
      _sockfd = newsock;
    }

    bool Accept(Tcp_Scoket& newsock)
    {
      struct sockaddr_in cliaddr;
      socklen_t len = sizeof(cliaddr);

      int newsockfd = accept(_sockfd, (struct sockaddr*)&cliaddr, &len);
      if(newsockfd < 0)
      {
        cerr << "accept error" << endl;
        return false;
      }
      
      newsock.Setsockfd(newsockfd);
      return true;
    }

    bool Recv(string& date)
    {
      char temp[1024] = {0};
      int ret = recv(_sockfd, temp, 1024, 0);
      if(ret == 0)
      {
        cout << "peer shutdown" << endl;
        return false;
      }
      else if(ret < 0)
      {
        cout << "recv error" << endl;
        return false;
      }

      date.assign(temp, ret);
      return true;
    }

    bool Send(const string& date)
    {
      int ret = send(_sockfd, date.c_str(), date.size(), 0);
      if(ret < 0)
      {
        cerr << "send error" << endl;
        return false;
      }
      return true;
    }

  private:
    int _sockfd;
};

void* thread_start(void* socket)
{
  Tcp_Scoket* sock= (Tcp_Scoket*)socket;
  // HTTP请求信息打印
  string date;
  sock->Recv(date);
  stringstream ss;
  ss << "客户端请求:" << endl << "[" << date << "]" << endl;
  cout << ss.str();

  // HTTP响应信息组织
  string frist = "HTTP/1.1 301 Not Find\r\n";             // 根据首行状态码来确定所需要的头部信息
  string blank_line = "\r\n";
  string text = "<html><head><meta charset='utf-8'><title>Superao</title></head><body><h1>Hello World!</h1><p>你好</p></body></html>";
  string header = "Content-Length: ";
  header += text.size();
  header += "\r\n";
  header += "Location: https://www.taobao.com/\r\n";      // 重定向

  stringstream sdate;
  sdate << frist << header << blank_line << text << endl;
  date.clear();
  date = sdate.str();

  sock->Send(date);
  return NULL;
}

int main()
{
  Tcp_Scoket litsock;
  CHECK_RET(litsock.Socket());
  CHECK_RET(litsock.Bind("192.168.248.129", 8000));
  CHECK_RET(litsock.Listen());
  while(1)
  {
    // 创建会话套接字
    Tcp_Scoket* handlesock = new Tcp_Scoket;
    litsock.Accept(*handlesock);

    // 创建线程处理数据
    //pthread_t tid = -1;
    //pthread_create(&tid, NULL, thread_start, (void*)handlesock);
    //pthread_detach(tid);
    thread th(thread_start, handlesock);
    th.detach();
  }

  return 0;
}
