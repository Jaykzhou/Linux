#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

int main()
{
  // 1. 调用系统调用                                  (创建子进程，程序替换)
  // system("date");                                  
  // system("stat ./udp_cli.cpp");
  
  // 2. 调用函数接口
  // time_t time(time_t *t);                          返回格林威治时间到现在所经过的秒数。
  time_t t;
  time(&t);
  
  // char *ctime(const time_t *timep);                把得到的秒数转化为当前时间  1. 直接返回字符串常量指针 
  // char *ctime_r(const time_t *timep, char *buf);                               2. 将字符串储存在buf中，返回buf的地址      
  char timestr[1024] = {0};
  ctime_r(&t, timestr);
  cout << timestr;

  return 0;
}
