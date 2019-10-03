// 学习条件变量的使用: 利用吃饭/做饭来实现线程之间的同步(时序合理性)
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// 默认没有饭(临界资源)
int is_have_a_noodles = 0;

// 互斥锁
pthread_mutex_t mutex;

// 条件变量
pthread_cond_t econd;
pthread_cond_t mcond;

void* epth_start()
{
  // 吃饭的人: 有饭就吃饭，没有饭则等待
  while(1)
  {
    pthread_mutex_lock(&mutex);
    // if(is_have_a_noodles == 0)             不支持并发，只有循环判断才能支持并发
    while(is_have_a_noodles == 0)
    {
      pthread_cond_wait(&econd, &mutex);   // 不同的角色等待在不同的队列中，否则会发生相互等待的情况(死等)  
    }

    printf("i eating a noodles\n");
    is_have_a_noodles = 0;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&mcond);
  }
}

void* mpth_start()
{
  // 做饭的人: 没有饭就做饭，有饭则等待
  while(1)
  {
    pthread_mutex_lock(&mutex);
    // if(is_have_a_noodles == 1)
    while(is_have_a_noodles == 1)
    {
      pthread_cond_wait(&mcond, &mutex);
    }

    printf("i making a noodles\n");
    is_have_a_noodles = 1;
    
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&econd);
  }
}

int main()
{

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&econd, NULL);
  pthread_cond_init(&mcond, NULL);

  // 吃饭的人
  pthread_t etid[4];
  for(int i = 0; i < 4; ++i)
  {
    int eret = pthread_create(&etid[i], NULL, epth_start, NULL);
    if(eret != 0)
    {
      perror("pthread create error");
      return -1;
    }
  }
  
  // 做饭的人
  pthread_t mtid[4];
  for(int i = 0; i < 4; ++i)
  {
    int mret = pthread_create(&mtid[i], NULL, mpth_start, NULL);
    if(mret != 0)
    {
      perror("pthread create error");
      return -1;
    }
  }

  // 线程等待
  for(int i = 0; i < 4; ++i)
  {
    pthread_join(mtid[i], NULL); 
    pthread_join(etid[i], NULL); 
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&econd);
  pthread_cond_destroy(&mcond);

  return 0;
}
