// 传输层基于 UDP 协议的服务端程序
// 数据传输流程:
//    1. 创建套接字(操作系统将进程与网卡之间建立联系)
//    2. 为套接字绑定地址信息(服务端的地址信息一般使用固定的地址，方便客户端发送) 
//    3. 接受数据(获取客户端的地址信息, 端口信息, 接受客户端的信息)
//    4. 发送数据(利用获取到的地址信息, 端口信息, 向客户端发送信息)
//    5. 关闭套接字
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>   
#include <arpa/inet.h>      
#include <netinet/in.h>      
using namespace std;

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.248.129 9000\n";
    return -1;
  }
  // 创建套接字
  // int socket(int domain, int type, int protocol);
  // domain: 地址域标识符(AF_INET:IPV4, AF_INET6:IPV6);
  // type: 套接字类型(SOCK_DGRAM:数据包套接字, SOCK_STREAM:字节流套接字);
  // protocol: 协议类型(IPPROTO_UDP, IPPROTO_TCP);
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sock < 0)
  {
    cerr << "socket error" << endl;
  }

  // 利用套接字描述符为套接字绑定地址信息
  // int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
  struct sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET; 
  sockaddr.sin_addr.s_addr = inet_addr(argv[1]);
  sockaddr.sin_port = htons(atoi(argv[2]));

  socklen_t len = sizeof(sockaddr);
  int ret = bind(sock, (struct sockaddr*)&sockaddr, len);
  if(ret < 0)
  {
    cerr << "bind error" << endl;
  }

  while(1)
  {
    char buf[1024] = {0};
    struct sockaddr_in sockaddr;
    socklen_t len = sizeof(sockaddr);
    // 接受数据                                            阻塞标志   客户端地址信息(输入输出)   地址长度(输入输出)
    // ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
    int ret = recvfrom(sock, buf, 1023, 0, (struct sockaddr*)&sockaddr, &len);
    if(ret < 0)
    {
      cerr << "recvfrom error" << endl;
      return -1;
    }
    cout << "cli say: " << buf << endl;

    memset(buf, 0, 1024);
    cin >> buf;
    // 发送数据
    // ssize_t sendto(int sockfd, void *buf, size_t len, int flags, struct sockaddr *dest_addr, socklen_t addrlen)
    ret = sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&sockaddr, len);
    if(ret < 0)
    {
      cerr << "sendto error" << endl;
      return -1;
    }
  }

  close(sock);
  return 0;
}

