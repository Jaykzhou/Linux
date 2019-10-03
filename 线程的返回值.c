// 学习获取线程的返回值
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* pth_start()
{
  sleep(3);
  char* ret = "i am student!";
  // return ret;           直接进行返回
  pthread_exit(ret);    // 将需要被返回的值传入到参数中，进行返回
}

int main()
{
  // 创建线程
  pthread_t tid;
  int ret = pthread_create(&tid, NULL, pth_start, NULL);
  if(ret != 0)
  {
    perror("create error!");
    return -1;
  }

  // 等待线程，获取返回值
  char* retval = NULL;          
  pthread_join(tid, (void**)&retval);     // 一级指针想要被修改就需要传入二级指针，但是直接传入二级指针会坑
  printf("pthread say : %s\n", retval);
  
  return 0;
}
