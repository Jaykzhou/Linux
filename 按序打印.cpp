// 利用三个线程，依次打印 1, 2, 3;
#include <iostream>
#include <thread>
#include <pthread.h>
using namespace std;

// 临界资源
int date = 1;
// 互斥锁
pthread_mutex_t mutex;
// 条件变量
pthread_cond_t ocond;
pthread_cond_t tcond;
pthread_cond_t thcond;

void one()
{
  pthread_mutex_lock(&mutex);
  if(date != 1)
  {
    pthread_cond_wait(&ocond, &mutex);
  }

  cout << "one";
  date++;
  pthread_mutex_unlock(&mutex);
  pthread_cond_broadcast(&tcond);
  pthread_cond_broadcast(&thcond);
}

void two()
{
  pthread_mutex_lock(&mutex);
  if(date != 2)
  {
    pthread_cond_wait(&tcond, &mutex);
  }

  cout << "two";
  date++;
  pthread_mutex_unlock(&mutex);
  pthread_cond_broadcast(&ocond);
  pthread_cond_broadcast(&thcond);
}

void three()
{
  pthread_mutex_lock(&mutex);
  if(date != 3)
  {
    pthread_cond_wait(&thcond, &mutex);
  }

  cout << "three";
  date++;
  pthread_mutex_unlock(&mutex);
  pthread_cond_broadcast(&ocond);
  pthread_cond_broadcast(&tcond);
}

int main()
{
  thread thone = thread(one);
  thread thtwo = thread(two);
  thread ththree = thread(three);
  
  thone.join();
  thtwo.join();
  ththree.join();

  cout << endl;

  return 0;
}
