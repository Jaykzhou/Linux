// 传输层基于 TCP 协议的服务端程序
// 数据传输流程:
//    1. 创建套接字(进程与网卡建立联系)
//    2. 绑定地址信息
//    3. 开始监听(获取客户端的 SYN 链接请求)
//    4. 获取已完成新链接(从已完成链接队列中获取已完成的新连接)
//    5. 接受数据
//    6. 发送数据
//    7. 关闭套接字
#include "tcp_socket.hpp"

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.248.129 9000" << endl;
    return -1;
  }
  // 创建Tcpsocket对象
  TcpSocket lissock;

  CHECK(lissock.Socket());

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);
  CHECK(lissock.Bind(ip, port));

  CHECK(lissock.Listen());

  TcpSocket sock;
  while(1)
  {
    // TcpSocket sock(临时对象，一轮循环结束会自动析构)
    // 获取已完成链接
    // Accept 函数是一个阻塞函数，当没有新的链接请求时，它就会挂起等待，直到有新的链接到来时，它才会获取到新的链接。
    bool flag = lissock.Accept(sock);
    if(flag == false)
    {
      continue;
    }
    // 接受数据
    string date;
    // recv 函数是一个阻塞函数。
    flag = sock.Recv(date);
    if(flag == false)
    {
      continue;
    }
    cout << "Cli say : " << date << endl;

    // 发送数据
    date.clear();
    cin >> date;
    sock.Send(date);
  }

  return 0;
}
