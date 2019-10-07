// 传输层基于 TCP 协议的客户端程序
// 数据传输流程:
//     1. 创建套接字
//     2. 绑定地址信息
//     3. 发送链接请求
//     4. 发送数据
//     5. 接受数据
//     6. 关闭套接字
#include "tcp_socket.hpp"

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.248.129 9000" << endl;
    return -1;
  }

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  TcpSocket sock;
  CHECK(sock.Socket());
  // CHECK(sock.Bind())       客户端通常不需要绑定地址信息
  CHECK(sock.Connect(ip, port));
  while(1)
  {
    string date;
    cin >> date;
    bool flag = sock.Send(date);
    if(flag == false)
    {
      return -1;
    }

    date.clear();
    flag = sock.Recv(date);
    if(flag == false)
    {
      return -1;
    }
    cout << "Srv say: " << date << endl;
  }

  return 0;
}
