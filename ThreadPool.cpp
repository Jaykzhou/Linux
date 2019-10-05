// 线程池:
// 1. 任务类
// 2. 线程池类
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <unistd.h>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define CMAX 10
#define TMAX 5

// 函数指针起别名(handle_fun)
typedef void(*handle_fun)(int date);

// 任务类
class Task
{
  private:
    int _date;                      // 处理数据
    handle_fun _handle_fun;         // 处理方法

  public:
    // 构造函数
    Task(int date, handle_fun fun) :_date(date), _handle_fun(fun) {}

    // 设置任务
    void SetTask(int date, handle_fun fun)
    {
      _date = date;
      _handle_fun = fun;
    }

    // 处理任务
    void Run()
    {
      return _handle_fun(_date);
    }
};

// 线程池类
class ThreadPool
{
  private:
    // 任务队列
    queue<Task> _queue;
    int _capacity;

    // 同步与互斥
    pthread_mutex_t _mutex;
    pthread_cond_t _mcond;
    pthread_cond_t _ccond;

    // 控制线程状态
    int _max_num;                 // 线程最大数量
    bool _flag;                    // 状态标记 
    int _cur_num;                 // 当前线程数量
    vector<thread> _thr_list;     // 线程操作句柄

  private:
    // 线程入口函数(消费者)
    bool thr_start()
    {
      while(1)
      {
        pthread_mutex_lock(&_mutex);
        while(_queue.empty())
        {
          if(_flag == true)
          {
            cout << "thread exit : " << pthread_self() << endl;
            _cur_num--;
            pthread_mutex_unlock(&_mutex);
            return true;
          }

          pthread_cond_wait(&_ccond, &_mutex);
        }

        Task tt = _queue.front();
        _queue.pop();
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_mcond);

        tt.Run();
      }
      
      return true;
    }
  
  public:
    ThreadPool(int capacity = CMAX, int max_num = TMAX) 
      :_capacity(capacity), _max_num(max_num)
      , _flag(false), _cur_num(0), _thr_list(max_num)
    {
      pthread_mutex_init(&_mutex, NULL);
      pthread_cond_init(&_mcond, NULL);
      pthread_cond_init(&_ccond, NULL);
    }

    ~ThreadPool()
    {
      pthread_mutex_destroy(&_mutex);
      pthread_cond_destroy(&_mcond);
      pthread_cond_destroy(&_ccond);
    }

    // 创建线程
    bool ThreadInit()
    {
      for(int i = 0; i < _max_num; ++i)
      {
        _thr_list[i] = thread(&ThreadPool::thr_start, this);
        _cur_num++;
        _thr_list[i].detach();
      }

      return true;
    }

    // 添加任务(生产者,生产数据)
    bool AddTask(Task tt)
    {
      pthread_mutex_lock(&_mutex);
      while((int)_queue.size() >= _capacity)
      {
        pthread_cond_wait(&_mcond, &_mutex);
      }

      _queue.push(tt);
      pthread_mutex_unlock(&_mutex);
      pthread_cond_signal(&_ccond);

      return true;
    }

    // 线程退出
    bool ThreadQuit()
    {
      pthread_mutex_lock(&_mutex);
      _flag = true;
      pthread_mutex_unlock(&_mutex);

      while(_cur_num > 0)
      {
        pthread_cond_broadcast(&_ccond);
        usleep(1000);
      }
      
      return true;
    }
};

// 处理数据,处理方法
void Handle_fun(int date)
{
  srand(time(0));

  int nsec = rand() % 5;

  stringstream ss;
  ss << "thread :" << pthread_self() << " processing a date :" << date;
  ss << " and sleep " << nsec << "s exit!" << endl;
  cout << ss.str();
  
  sleep(nsec);
}

int main()
{
  ThreadPool pool;
  pool.ThreadInit();

  for(int i = 0; i < 10; ++i)
  {
    pool.AddTask(Task(i, Handle_fun));
  }

  pool.ThreadQuit();

  return 0;
}
