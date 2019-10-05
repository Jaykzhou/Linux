// 利用多线程答应奇数，偶数:(支持并发)
// 线程A(5 个):打印奇数
// 线程B(5 个):打印偶数
#include <iostream>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <sstream>
using namespace std;

pthread_mutex_t mut;
pthread_cond_t Acond;
pthread_cond_t Bcond;

// 全局数据(临界资源)
int date = 0;

void tha_start()
{
  while(1)
  {
    pthread_mutex_lock(&mut);
    while(date % 2 == 0)
    {
      pthread_cond_wait(&Acond, &mut);
    }

    stringstream ss;
    ss << "线程A 打印了一个数据: " << date++ << endl;
    cout << ss.str();
    pthread_mutex_unlock(&mut);
    pthread_cond_signal(&Bcond);
  }

  return ;
}

void thb_start()
{
  while(1)
  {
    pthread_mutex_lock(&mut);
    while(date % 2 != 0)
    {
      pthread_cond_wait(&Bcond, &mut);
    }

    stringstream ss;
    ss << "线程B 打印了一个数据: " << date++ << endl;
    cout << ss.str();
    pthread_mutex_unlock(&mut);
    pthread_cond_signal(&Acond);
  }

  return ;
}

int main()
{
  // 线程A
  thread thA[5];
  for(int i = 0; i < 5; ++i)
  {
    thA[i] = thread(tha_start);
    //thA[i].detach();
  }

  // 线程B
  thread thB[5];
  for(int i = 0; i < 5; ++i)
  {
    thB[i] = thread(thb_start);
    //thB[i].detach();
  }

  for(int i = 0; i < 5; ++i)
  {
    thA[i].join();
    thB[i].join();
  }

  return 0;
}


