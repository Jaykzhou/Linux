// 使用信号量实现生产者与消费者模型
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <queue>
#include <sstream>
using namespace std;

// 信号量 -> 线程安全的队列
class Semqueue
{
    public:
        ~Semqueue()
        {
            sem_destroy(&sempro);
            sem_destroy(&semcon);
        }

        void Init(int provalue, int convalue)
        {
            sem_init(&sempro, 0, provalue);
            sem_init(&semcon, 0, convalue);
        }

        // 生产者线程函数
        void pro_handler(void* handler(void*), void* arg)
        {
            for(int i = 0; i < 4; ++i)
            {
                pthread_create(&ptid[i], NULL, handler, arg);
            }
        }

        // 消费者线程函数
        void con_handler(void* handler(void*), void* arg)
        {
            for(int i = 0; i < 4; ++i)
            {
                pthread_create(&ctid[i], NULL, handler, arg);
            }
        }

        // 回收线程
        void join_handler()
        {
            //  回收生产者
            for(int i = 0; i < 4; ++i)
            {
                pthread_join(ptid[i], NULL);
            }

            // 回收消费者
            for(int i = 0; i < 4; ++i)
            {
                pthread_join(ctid[i], NULL);
            }
        }

        // 存放数据
        void QueuePush(const string& str)
        {
            sem_wait(&sempro);          // 判断当前是否有资源，有资源，则计数-1，否则进行等待
            que.push(str);
            sem_post(&semcon);          // 发布信号量，对资源计数进行+1操作
        }
        
        // 取出数据
        void QueuePop(string& str)
        {
            sem_wait(&semcon);
            str = que.front();
            que.pop();
            sem_post(&sempro);
        }
        
    private:
        sem_t sempro;          // 生产者 -> 空闲位置
        sem_t semcon;          // 消费者 -> 所需数据

        pthread_t ptid[4];
        pthread_t ctid[4];

        queue<string> que;
};

// 共享数据
int value = 0;

// 互斥锁
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

// 生产者
void* phandler(void* arg)
{
    Semqueue* sque = (Semqueue*)arg;
    while(1)
    {
        pthread_mutex_lock(&mut);
        value++;
        stringstream ss;
        ss << "当前数据为: " << value << endl;
        pthread_mutex_unlock(&mut);
        cout << "生产者生产了一个数据，" << ss.str();

        sque->QueuePush(ss.str());

        int t = rand() % 3;
        sleep(t);
    }
}

// 消费者
void* chandler(void* arg)
{
    Semqueue* sque = (Semqueue*)arg;
    while(1)
    {
        string str;
        sque->QueuePop(str);
        cout << "消费者获取了一个数据，" << str;

        int t = rand() % 3;
        sleep(t);
    }
}

int main()
{
    srand(time(0));

    Semqueue sque;
    sque.Init(5, 0);     // 信号量初始值 -> 空闲计数: 5, 资源计数: 0.

    sque.pro_handler(phandler, (void*)&sque);
    sque.con_handler(chandler, (void*)&sque);
    
    sque.join_handler();

    return 0;
}
