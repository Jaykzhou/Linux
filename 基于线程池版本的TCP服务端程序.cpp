// 基于线程池版本的TCP协议的服务端程序
// 大体设计思路:
// 1. 任务类:(TCP请求)
//      成员变量: 1. 数据(tcpsokcet 对象)  2. 回调函数(获取已完成新连接)
//      成员函数: 1. 设置回调函数  2. 运行函数
// 2. 线程池类:
//      成员变量: 1. 线程的操作句柄  2. 线程安全的队列  3. 条件变量  4. 当前线程数量  5. 退出标记
//      成员函数: 1. 初始化线程(创建大量线程)  2. 添加任务结点  3. 处理任务  4. 线程退出 
#include <iostream>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <unistd.h>
#include "tcp_socket.hpp"

#define THREAD_MAX 10
#define QUEUENODE 20
using namespace std;

// 任务类
typedef bool(*handle)(TcpSocket* tcpsock);
class Task 
{
  public:
    Task(handle fun, TcpSocket* sock) :_tcpsock(sock), _handle(fun) {}

    // 设置回调函数
    bool Sethandle(handle fun, TcpSocket* sock)
    {
      _handle = fun;
      _tcpsock = sock;
      return true;
    }

    // 运行函数
    void Run()
    {
      _handle(_tcpsock);
    }

  private:
    TcpSocket* _tcpsock;
    handle _handle;
};


// 线程池类
class TcpThreadPool
{
  public:
    TcpThreadPool(int threadmax = THREAD_MAX, int capacity = QUEUENODE) 
         :_listid(threadmax), _capacity(capacity), _curnum(0), _flag(0), _threadmax(threadmax)
    {
      pthread_mutex_init(&_mutex, NULL);
      pthread_cond_init(&_mcond, NULL);
      pthread_cond_init(&_ccond, NULL);
    }

    ~TcpThreadPool()
    {
      pthread_mutex_destroy(&_mutex);
      pthread_cond_destroy(&_mcond);
      pthread_cond_destroy(&_ccond);
    }

    bool InitThread()
    {
      for(int i = 0; i < _threadmax; ++i)
      {
        _listid[i] = thread(&TcpThreadPool::Thead_statr, this);        // 这里的使用方法需要注意
        _curnum++;
        _listid[i].detach();
      }

      return true;
    }

    // 生产者
    bool AddTask(Task task)
    {
      // 线程安全队列
      pthread_mutex_lock(&_mutex);
      while((int)_queue.size() >= _capacity)               // 循环检测队列中是否有空闲位置
      {
        pthread_cond_wait(&_ccond, &_mutex);               // 将 unlock, wait, lock 集于一体
      }

      // 满足条件
      _queue.push(task);
      pthread_mutex_unlock(&_mutex);
      pthread_cond_signal(&_ccond);

      return true;
    }

    void ThreadExit()
    {
      pthread_mutex_lock(&_mutex);
      _flag = 1;
      pthread_mutex_unlock(&_mutex);

      while(_curnum > 0)
      {
        pthread_cond_broadcast(&_ccond);
      }
    }

  private:
    // 消费者
    bool Thead_statr()
    {
      while(1)
      {
        pthread_mutex_lock(&_mutex);
        if(_flag == 1)
        {
          cout << "threadID: " << pthread_self() << "exit" << endl;
          _curnum--;
          pthread_mutex_unlock(&_mutex);

          return true;
        }
        while((int)_queue.size() <= 0)                                 // 循环检测是否有数据可进行处理
        {
          pthread_cond_wait(&_ccond, &_mutex);
        }

        // 满足条件
        Task tt = _queue.front();
        _queue.pop();

        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_mcond);

        tt.Run();
      }

      return true;
    }

  private:
    vector<thread> _listid;
    queue<Task> _queue;
    int _capacity;

    pthread_mutex_t _mutex;
    pthread_cond_t _mcond;
    pthread_cond_t _ccond;

    int _curnum;
    int _flag;
    int _threadmax;
};


bool HandleFun(TcpSocket* sock)
{
  string date;
  // 处理任务
  while(1)
  {
    sock->Recv(date);
    cout << date << endl;
    date.clear();
    cin >> date;
    sock->Send(date);
  }

  return true;
}

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    cout << "./a.out 192.168.248.129 9000" << endl;
    return -1;
  }

  // 将已完成连接加入到线程池任务队列中
  TcpSocket lissock;
  TcpThreadPool thpool;
  thpool.InitThread();

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);

  lissock.Socket();
  lissock.Bind(ip, port);
  lissock.Listen();

  while(1)
  {
    TcpSocket* sock = new TcpSocket;
    lissock.Accept(*sock);
    Task tt(HandleFun, sock);
    thpool.AddTask(tt);
  }

  return 0;
}
