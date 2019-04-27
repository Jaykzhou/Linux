// 体会竞态条件 --- 理解函数的可重入和不可重入。

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


int a = 0, b = 0;
int Text() {        // 不可重入函数。

  a++;
  sleep(4);         // 证明 a++ b++ 并非原子操作,在这期间可能会被其他执行流打断,例如:信号来了。
  b++;
  return a + b;
}

void sigcb(int signo) {

  printf("signal ----- %d\n", Text());
}
int main() {

  signal(SIGINT,sigcb);
  printf("main ----- %d\n ",Text());
  return 0;
}
