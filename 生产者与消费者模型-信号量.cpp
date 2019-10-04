// 学习 posix 标准信号量, 使用它的相关接口实现生产者与消费者模型
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>    // posix 标准信号量
using namespace std;

#define MAX 10

// 数据缓冲场所
class RingQueue
{
  public:
    RingQueue(int capacity = MAX)
      :_queue(MAX), _capacity(capacity), _step_start(0), _step_end(0)
    {
      // int sem_init(sem_t *sem, int pshared, unsigned int value); 
      sem_init(&_lock, 0, 1);
      sem_init(&_msem, 0, _capacity);
      sem_init(&_csem, 0, 0);
    }

    ~RingQueue()
    {
      sem_destroy(&_lock);
      sem_destroy(&_msem);
      sem_destroy(&_csem);
    }

    bool Push(int date)
    {
      sem_wait(&_msem);      // 信号量内部判断是否有剩余空间
      sem_wait(&_lock);      // 计数 - 1

      _queue[_step_end] = date;
      _step_end = (_step_end + 1) % _capacity;

      sem_post(&_lock);      // 计数 + 1
      sem_post(&_csem);

      return true;
    }

    bool Pop(int* date)
    {
      sem_wait(&_csem);      // 信号量内部判断是否有数据存放
      sem_wait(&_lock);

      *date = _queue[_step_start];
      _step_start = (_step_start + 1) % _capacity;

      sem_post(&_lock);
      sem_post(&_msem);

      return true;
    }

  private:
    // 循环队列
    vector<int> _queue;
    int _capacity;
    int _step_start;
    int _step_end;

    // 线程安全
    sem_t _lock;    // 锁信号量
    sem_t _msem;    // 生产者信号量
    sem_t _csem;    // 消费者信号量
};

void mth_start(RingQueue& queue)
{
  int date = 0;
  while(1)
  {
    queue.Push(date);
    cout << "making a date :" << date++ << endl;
  }
}

void cth_start(RingQueue& queue)
{
  int date = 0;
  while(1)
  {
    queue.Pop(&date);
    cout << "geting a date :" << date << endl;
  }
}

int main()
{

  RingQueue queue;

  // C++11 
  vector<thread> mth(4);
  vector<thread> cth(4);

  for(int i = 0; i < 4; ++i)
  {
    mth[i] = thread(mth_start, ref(queue));                 // 匿名对象,赋值构造
  }

  for(int i = 0; i < 4; ++i)
  {
    cth[i] = thread(cth_start, ref(queue));                 // 也可以传指针
  }

  for(int i = 0; i < 4; ++i)
  {
    mth[i].join();
    cth[i].join();
  }

  return 0;
}
