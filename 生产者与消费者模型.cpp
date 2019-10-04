// 利用互斥锁与条件变量实现生产者与消费者模型
// 角色关系:
// 生产者与生产者: 互斥
// 消费者与消费者: 互斥
// 生产者与消费者: 同步与互斥
// 数据缓冲: 队列
// 生产者与消费者模型总结: 一个场所，两种角色，三种关系
#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
using namespace std;

#define MAX 10

// 数据缓冲队列
class BlockQueue
{
  private:
    int _capacity;
    pthread_mutex_t _mutex;
    pthread_cond_t _mcond;     // 生产者
    pthread_cond_t _ccond;     // 消费者
    queue<int> q;

  public:
    BlockQueue(int capacity = MAX) 
      :_capacity(capacity)
    {
      pthread_mutex_init(&_mutex, NULL);
      pthread_cond_init(&_mcond, NULL);
      pthread_cond_init(&_ccond, NULL);
    }

    ~BlockQueue()
    {
      pthread_mutex_destroy(&_mutex);
      pthread_cond_destroy(&_mcond);
      pthread_cond_destroy(&_ccond);
    }

    bool Push(int date)
    {
      pthread_mutex_lock(&_mutex);
      while((int)q.size() >= _capacity)
      {
        pthread_cond_wait(&_mcond, &_mutex);
      }

      q.push(date);
      pthread_mutex_unlock(&_mutex);
      pthread_cond_signal(&_ccond);

      return true;
    }

    bool Pop(int* date)
    {
      pthread_mutex_lock(&_mutex);
      while((int)q.size() <= 0)
      {
        pthread_cond_wait(&_ccond, &_mutex);
      }

      *date = q.front();
      q.pop();
      pthread_mutex_unlock(&_mutex);
      pthread_cond_signal(&_mcond);
 
      return true;
    }
};

void* mpth_start(void* arg)
{
  int date = 0;
  BlockQueue* q = (BlockQueue*)arg;
  
  while(1)
  {
    q->Push(date);
    cout << "生产了一个数据: " << date++ << endl;     // 这里打印的时候会被中断，因此在终端会看到假象
  }
  
  return NULL;
}

void* cpth_start(void* arg)
{
  int date = 0;
  BlockQueue* q = (BlockQueue*)arg;
  
  while(1)
  {
    q->Pop(&date);
    cout << "获取到了一个数据: " << date << endl;
  }
  
  return NULL;
}

int main()
{
  BlockQueue queue;

  pthread_t mtid[4];
  for(int i = 0; i < 4; ++i)
  {
    pthread_create(&mtid[i], NULL, mpth_start, (void*)&queue);
  }

  pthread_t ctid[4];
  for(int i = 0; i < 4; ++i)
  {
    pthread_create(&ctid[i], NULL, cpth_start, (void*)&queue);
  }

  for(int i = 0; i < 4; ++i)
  {
    pthread_join(mtid[i], NULL);
    pthread_join(ctid[i], NULL);
  }

  return 0;
}
