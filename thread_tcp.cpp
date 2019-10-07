// 传输层基于 TCP 协议的服务端程序
// 数据传输流程:
//    1. 创建套接字(进程与网卡建立联系)
//    2. 绑定地址信息
//    3. 开始监听(获取客户端的 SYN 链接请求)
//    4. 获取已完成新链接(从已完成链接队列中获取已完成的新连接)
//    5. 接受数据
//    6. 发送数据
//    7. 关闭套接字
#include <pthread.h>
#include "tcp_socket.hpp"

void* thread_start(void* arg)
{
  TcpSocket* sock = (TcpSocket*)arg;
  while(1)
  {
    // 接受数据
    string date;
    // recv 函数是一个阻塞函数。
    int flag = sock->Recv(date);
    if(flag == false)
    {
      delete sock;
      return NULL;
    }
    cout << "Cli say : " << date << endl;

    // 发送数据
    date.clear();
    cin >> date;
    sock->Send(date);
  }

  return NULL;
}

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

  while(1)
  {
    // 1. TcpSocket sock(临时对象，一轮循环结束会自动析构，从而导致原有的空间被释放，无法使用)
    // 2. 堆上申请空间可以一直使用，每块空间中有自己的套接字文件描述信息，
    //    供不同的线程来使用，从而实现了一个服务器与多个客户端进行通信。
    //    lissock: 获取新连接   sockptr: 负责与不同的客户端进行通信
    TcpSocket* sockptr = new TcpSocket;  
    
    // 获取已完成新链接
    // Accept 函数是一个阻塞函数，当没有新的链接请求时，它就会挂起等待，直到有新的链接到来时，它才会获取到新的链接。
    bool flag = lissock.Accept(*sockptr);
    if(flag == false)
    {
      continue;
    }

    // 创建线程，进行数据的处理
    pthread_t tid = -1;
    pthread_create(&tid, NULL, thread_start, (void*)sockptr);
    pthread_detach(tid);
  }

  return 0;
}
