// 利用互斥锁实现线程安全操作
// 以 "黄牛抢票" 为例子实现线程安全的抢票
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static int num = 10000;  // 临界资源
pthread_mutex_t mutex;   // 全局锁

void* Buy(void* i)
{
  int id = (int)i;

  while(1)
  {
    pthread_mutex_lock(&mutex);
    if(num > 0)
    {
      printf("第 %d 号黄牛抢到了 %d 号票\n", id, num--);
      pthread_mutex_unlock(&mutex);
    }
    else 
      break;
  }
  pthread_mutex_unlock(&mutex);

  return NULL;
}

int main()
{
  // 初始化锁
  pthread_mutex_init(&mutex, NULL);

  // 主线程创建线程抢票
  pthread_t tid[5] = {0};
  int i = 0;
  for(i = 0; i < 5; ++i)
  {
    int ret = pthread_create(&tid[i], NULL, Buy, (void*)i);
    if(ret != 0)
    {
      perror("pthread create error!");
      return -1;
    }
  }

  // 线程等待
  for(i = 0; i < 5; ++i)
  {
    pthread_join(tid[i], NULL);
  }
  
  // 销毁锁
  pthread_mutex_destroy(&mutex);

  return 0;
}
