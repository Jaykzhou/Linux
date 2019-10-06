// 传输层基于 UDP 协议的客户端程序
// 数据传输流程:
//    1. 创建套接字
//    2. 为套接字绑定地址信息(一般不需要自行绑定, 当我们没有绑定时, 操作系统会自动进行绑定)
//       (如果我们绑定地址, 那么只能启动一个客户端程序, 因为一个端口只能被一个进程使用, 因此一般不进行绑定)
//    3. 发送数据
//    4. 接受数据
//    5. 关闭套接字
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

class Socket_Cli
{
  public:
    Socket_Cli():_sock_id(-1) {}
    ~Socket_Cli()
    {
      close(_sock_id);
    }

    bool Socket()
    {
      _sock_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if(_sock_id < 0)
      {
        cerr << "socket error" << endl;
        close(_sock_id);
        _sock_id = -1;

        return false;
      }

      return true;
    }

    bool Bind(const string& ip, uint16_t prot)
    {
      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      // inet_addr(): 将点分十进制的字符串地址转化为 uint32_t 地址
      addr.sin_addr.s_addr = inet_addr(ip.c_str());
      // 将主机字节序转化为网络字节序(避免数据二义)
      addr.sin_port = htons(prot);

      socklen_t len = sizeof(addr);
      int ret = bind(_sock_id, (struct sockaddr*)&addr, len);
      if(ret < 0)
      {
        cerr << "bind error" << endl;
        return false;
      }
      
      return true;
    }

    bool Recv(string& ip, uint16_t& prot, string& buf)
    {
      struct sockaddr_in sockaddr;

      socklen_t len = sizeof(sockaddr);
      char temp[1024] = {0};
      int retlen = recvfrom(_sock_id, temp, 1024, 0, (struct sockaddr*)&sockaddr, &len);
      if(retlen < 0)
      {
        cerr << "recvfrom error" << endl;
        return false;
      }

      // 将IP地址信息转化为点分十进制的字符串IP地址。
      ip = inet_ntoa(sockaddr.sin_addr);
      prot = ntohs(sockaddr.sin_port);

      buf.assign(temp, retlen);
      return true;
    }

    bool Send(const string& ip, uint16_t prot, string buf)
    {
      struct sockaddr_in sockaddr;
      sockaddr.sin_family = AF_INET;
      // inet_addr(): 将点分十进制的字符串地址转化为 uint32_t 地址
      sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
      // 将主机字节序转化为网络字节序(避免数据二义)
      sockaddr.sin_port = htons(prot);

      socklen_t len = sizeof(sockaddr);
      int ret = sendto(_sock_id, buf.c_str(), buf.size(), 0, (struct sockaddr*)&sockaddr, len);
      if(ret < 0)
      {
        cout << "sendto error" << endl;
        return false;
      }

      return true;
    }

  private:
    int _sock_id;
};

#define CHECK(p) if((p) == false) {return -1;}

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.248.129 9000" << endl;
    return -1;
  }

  string srv_ip = argv[1];
  uint16_t srv_prot = atoi(argv[2]);

  Socket_Cli sock;
  CHECK(sock.Socket());
  //CHECK(sock.Bind("192.168.248.129", 8000));
 
  while(1)
  {
    string str;
    cin >> str;
    CHECK(sock.Send(srv_ip, srv_prot, str));

    str.clear();
    CHECK(sock.Recv(srv_ip, srv_prot, str));
    cout << "Srv say: " << str << endl;
  }

  return 0;
}
