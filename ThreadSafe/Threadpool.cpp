// 线程池
#include <iostream>
#include <queue>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#define PTHREADMAX 100
#define PTHREADMIN 4
#define STEP 5

typedef void* (*handler)(void*);                  // 返回值为 void*，参数为 void* 的函数指针

class ThreadPoolTask
{
    public:
        void SetTask(handler h, void* a)
        {
            myhandler = h;
            arg = a;
        }

        bool RunTask()
        {
            return myhandler(arg) == NULL ? true : false;
        }

    private:
        void* arg;
        handler myhandler;
};

class ThreadPool
{
    public:
        // 初始化: 线程调用参数，线程数量，资源初值，空间初值
        void ThreadPool_Init(void* arg, int threadnum = 5, int r = 0, int i = 10)
        {
            pthread_mutex_init(&quemutex, NULL);
            pthread_mutex_init(&threadmutex, NULL);

            pthread_cond_init(&pcond, NULL);
            pthread_cond_init(&ccond, NULL);

            resource = r;
            idle = i;

            threadmax = PTHREADMAX;
            threadmin = PTHREADMIN;
            changestep = STEP;

            curthread = threadnum;
            busythread = 0;
            threadflag = false;

            normal.resize(threadnum);
            // 创建普通线程
            for(int i = 0; i < threadnum; ++i)
            {
                pthread_create(&normal[i], NULL, normalhandler, arg);
                pthread_detach(normal[i]);
            }

            // 管理者线程
            pthread_create(&manage, NULL, managehandler, arg);
            pthread_detach(manage);

            flag = false;
        }

        // 压入任务
        void QueuePush(ThreadPoolTask& task)
        {
            pthread_mutex_lock(&quemutex);
            while(idle <= 0)
            {
                pthread_cond_wait(&pcond, &quemutex);
            }

            taskqueue.push(task);
            resource++;
            idle--;
            pthread_mutex_unlock(&quemutex);
            pthread_cond_signal(&ccond);
        }
        
        // 取出任务
        void QueuePop(ThreadPoolTask& task)
        {
            pthread_mutex_lock(&quemutex);
            while(resource <= 0)
            {
                pthread_cond_wait(&ccond, &quemutex);

                // 唤醒后，检测两个退出标记，判断是否需要退出
                if(threadflag && curthread > threadmin)           // 保证减少线程后，线程池中的数量不能少于最小值
                {
                    pthread_mutex_unlock(&quemutex);
                    pthread_mutex_lock(&threadmutex);
                    curthread--;
                    pthread_mutex_unlock(&threadmutex);
                    cout << pthread_self() << "线程退出！" << endl;
                    pthread_exit(NULL); 
                }
                else if(curthread <= threadmin)                   // 销毁线程后，将标记位置为 false
                    threadflag = false;
                else if(flag)
                {
                    pthread_mutex_unlock(&quemutex);
                    pthread_mutex_lock(&threadmutex);
                    curthread--;
                    pthread_mutex_unlock(&threadmutex);
                    cout << pthread_self() << "线程退出！" << endl;
                    pthread_exit(NULL); 
                }
            }

            task = taskqueue.front();
            taskqueue.pop();
            idle++;
            resource--;

            pthread_mutex_unlock(&quemutex);
            pthread_cond_signal(&pcond);

            // 当线程取出任务后，执行任务时，此时忙碌线程数+1
            pthread_mutex_lock(&threadmutex);
            busythread++;
            pthread_mutex_unlock(&threadmutex);
        }

        void ThreadPool_Destroy()
        {
            flag = true;        // 注: 最好加锁。由于当前只在主线程中使用，因此可以在逻辑上避免竞态条件的产生

            while(curthread > 0) 
            {
                pthread_cond_signal(&ccond);
                usleep(100);
            }
        }

        void ThreadPool_Print()
        {
            stringstream ss;
            ss << "--------------------------------" << endl;
            ss << "当前线程池运行状态如下: " << endl;
            ss << "    线程总数: " << curthread << endl;
            ss << "    忙碌线程: " << busythread << endl;
            ss << "--------------------------------" << endl;

            cout << ss.str();
        }

    private:
        // 消费者 -> 取出任务 -> 执行任务 -> 空闲状态
        static void* normalhandler(void* arg)
        {
            ThreadPool* threadpool = (ThreadPool*)arg;
            while(1)
            {
                if(threadpool->flag == false)
                {
                    ThreadPoolTask task;
                    threadpool->QueuePop(task);

                    task.RunTask();

                    // 处理完任务后，此时忙碌线程数-1
                    pthread_mutex_lock(&threadpool->threadmutex);
                    threadpool->busythread--;
                    pthread_mutex_unlock(&threadpool->threadmutex);
                }
                // 当前线程池需要被销毁，因此处理完任务后直接退出
                else 
                {
                    pthread_mutex_lock(&threadpool->threadmutex);
                    threadpool->curthread--;
                    pthread_mutex_unlock(&threadpool->threadmutex);
                    cout << pthread_self() << "线程退出！" << endl;
                    pthread_exit(NULL); 
                }
            }
        }

        // 管理者线程
        static void* managehandler(void* arg)
        {
            ThreadPool* threadpool = (ThreadPool*)arg;
            while(1)
            {
                sleep(1);                                              // 每 1 秒检测一次线程池的运行情况

                cout << "开始监控线程池！" << endl;
                if(threadpool->flag == true)
                    break;

                pthread_mutex_lock(&threadpool->threadmutex);
                int curthreadnum = threadpool->curthread;
                int busythread = threadpool->busythread;
                if(busythread * 1.0 / curthreadnum * 1.0 >= 0.75)              // 超过 75% 的线程在忙碌时，增加线程池中的线程数
                {
                    for(int i = 0; i < threadpool->changestep; ++i)
                    {
                        if(threadpool->curthread < threadpool->threadmax)      // 保证增加线程后，线程池中的数量不能超过最大值
                        {
                            pthread_t tid;
                            int ret = pthread_create(&tid, NULL, normalhandler, arg);
                            threadpool->normal.push_back(tid);
                            if(ret == 0)
                                threadpool->curthread++;
                        }
                        else 
                            break;
                    }
                }

                if(busythread * 1.0 / curthreadnum <= 0.2)               // 只有少数线程在忙碌时，销毁线程池中的线程数
                {
                    threadpool->threadflag = true;
                    for(int i = 0; i < threadpool->changestep; ++i)
                    {
                        pthread_cond_signal(&threadpool->ccond);         // 唤醒后，由于队列中没有数据，线程则直接退出
                    }
                }
                pthread_mutex_unlock(&threadpool->threadmutex);
            }

            cout << "管理者线程已退出！" << endl;
            pthread_exit(NULL);
        }

    private:
        // 线程安全的任务队列
        pthread_mutex_t quemutex;
        int resource;
        int idle;
        pthread_cond_t pcond;
        pthread_cond_t ccond;
        queue<ThreadPoolTask> taskqueue;

        // 控制线程池
        int threadmax;
        int threadmin;
        int curthread;
        int busythread;
        int changestep;
        int threadflag;
        pthread_mutex_t threadmutex;

        // 记录线程ID
        vector<pthread_t> normal;
        pthread_t manage;

        // 线程池退出标记
        bool flag;
};

// 全局数据
int num = 0;

// 数据处理方法
void* datahandler(void* arg)
{
    int* data = (int*)arg;

    cout << "线程处理任务中！---------------------" << endl;
    sleep(10);

    return NULL;
}

ThreadPoolTask task;

// 生产者
void* phandler(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    int n = 10;
    while(n--)
        pool->QueuePush(task);

    pthread_exit(NULL);
}

int main()
{
    // 测试用例
    ThreadPool pool;
    pool.ThreadPool_Init(&pool);
    task.SetTask(datahandler, &num);

    for(int i = 0; i < 4; ++i)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, phandler, &pool);
        pthread_detach(tid);
    }
 
    //int n = 10;
    while(1)
    {
        sleep(1);
        pool.ThreadPool_Print();
    }

    //pool.ThreadPool_Destroy();

    return 0;
}
