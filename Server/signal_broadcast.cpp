// 测试 signal 与 broadcast 的区别:
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <queue>
using namespace std;

queue<int> que;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* pro_handler(void* )
{
    sleep(3);
    pthread_mutex_lock(&mutex);
    que.push(1);
    pthread_mutex_unlock(&mutex);

    //pthread_cond_signal(&cond);
    pthread_cond_broadcast(&cond);          // "惊群" 现象

    return NULL;
}

void* con_handler(void* )
{
    pthread_mutex_lock(&mutex);
    while(que.size() == 0)
    {
        pthread_cond_wait(&cond, &mutex);
        cout << "消费者线程被唤醒: " << pthread_self() << endl;
    }

    int data = que.front();
    que.pop();
    cout << data << "-----------" << pthread_self() << endl;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    pthread_t pro;
    pthread_create(&pro, NULL, pro_handler, NULL);

    pthread_t con[4];
    for(int i = 0; i < 4; ++i)
    {
        pthread_create(&con[i], NULL, con_handler, NULL);
    }

    pthread_join(pro, NULL);
    for(int i = 0; i < 4; ++i)
    {
        pthread_join(con[i], NULL);
    }

    return 0;
}
