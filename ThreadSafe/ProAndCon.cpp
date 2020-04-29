// 基于互斥锁与条件变量实现的生产者与消费者模型
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include <string>
#include <sstream>
using namespace std;

// 线程安全的队列
class SafeQueue
{
    public:
        SafeQueue()
        {
            resource = 0;
            idle = 10;
             
            pthread_mutex_init(&mutex, NULL);
            pthread_cond_init(&pcond, NULL);
            pthread_cond_init(&ccond, NULL);
        }

        ~SafeQueue()
        {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&pcond);
            pthread_cond_destroy(&ccond);
        }

        // 添加元素
        void PushQueue(string& data)
        {
            pthread_mutex_lock(&mutex);
            while(idle <= 0)
            {
                pthread_cond_wait(&pcond, &mutex);
            }

            // 当前有空间可以添加元素
            que.push(data);
            resource++;
            idle--;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&ccond); 
        }

        // 消耗元素
        void PopQueue(string& data)
        {
            pthread_mutex_lock(&mutex);
            while(resource <= 0)
            {
                pthread_cond_wait(&ccond, &mutex);
            }

            // 当前有资源可以被消耗
            data = que.front();
            que.pop();
            idle++;
            resource--;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&pcond);
        }

        // 生产者
        void Producer(void* handler(void*), void* arg)
        {
            for(int i = 0; i < 10; ++i)
            {
                pthread_create(&ptid[i], NULL, handler, arg);
            }
        }

        // 消费者
        void Consumer(void* handler(void*), void* arg)
        {
            for(int i = 0; i < 10; ++i)
            {
                pthread_create(&ctid[i], NULL, handler, arg);
            }
        }

        // 回收线程
        void Destroy()
        {
            // 回收生产者
            for(int i = 0; i < 10; ++i)
            {
                pthread_join(ptid[i], NULL);
            }

            // 回收消费者
            for(int i = 0; i < 10; ++i)
            {
                pthread_join(ctid[i], NULL);
            }
        }

    private:
        queue<string> que;
        pthread_mutex_t mutex;
        pthread_cond_t pcond;
        pthread_cond_t ccond;

        int resource;
        int idle;

        pthread_t ptid[10];
        pthread_t ctid[10];
};

// 生产者
void* phandler(void* arg)
{
    SafeQueue* squeue = (SafeQueue*)arg;
    while(1)
    {
        string data = "Hello World!!!" ;
        squeue->PushQueue(data);
        stringstream ss;
        ss << "生产者生产了一个数据！！！" << endl;
        cout << ss.str();
        sleep(1);
    }
}

// 消费者
void* chandler(void* arg)
{
    SafeQueue* squeue = (SafeQueue*)arg;
    while(1)
    {
        string data;
        squeue->PopQueue(data);
        stringstream ss;
        ss << "消费者获得了一个数据: " << data << endl;
        cout << ss.str();
        sleep(1);
    }
}

int main()
{
    SafeQueue squeue;
    squeue.Producer(phandler, (void*)&squeue);
    squeue.Consumer(chandler, (void*)&squeue);

    squeue.Destroy();

    return 0;
}
