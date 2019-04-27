// 修改 SIGCHLD 信号,避免一下问题
// 1.产生僵尸进程。
// 2.父进程死等,浪费了父进程资源。

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void sigcd(int signo) {

  // 有子进程退出,返回值将大于 0(子进程PID).
  while(waitpid(-1, NULL, WNOHANG) > 0){

    // 父进程成功接收到子进程退出信号。
    printf("childprocess success");

  }
}

int main() {

  // 将原来 SIGCHLD 的 处理方式修改为等待子进程退出。
  signal(SIGCHLD,sigcd);

  // 创建子进程
  int pid = fork();

  if(pid == 0) {

    sleep(3);
    exit(0);
  }
  else if(pid > 0) {

    // 父进程做其他任务
    while(1) {

      printf("学习中~~~\n");
      sleep(1);
    }

  }

  return 0;
}
