// 简易版网络计算器客户端
#include "tcp_cala.hpp"

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.248.129 9000" << endl;
    return -1;
  }

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  Tcp_socket csock;
  csock.Socket();
  csock.Connect(ip, port);

  while(1)
  {
    string numstr;
    cin >> numstr;
    csock.Send(numstr);

    csock.Recv(numstr);
    cout << "计算结果为: " <<  numstr << endl;
  }

  return 0;
}
