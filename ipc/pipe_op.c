// 管道符功能的实现:ps -ef | grep pipe
// 父进程创建管道,创建两个进程。
// 子进程1:用于实现'ps -ef'
// 子进程2:用于实现'grep pipe'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>


int main () {

  // 创建管道文件
  int pipefd[2];
  int ret = pipe(pipefd);
  if(ret < 0) {

    // 管道创建失败
    perror("pipe error");
    return -1;
  }

  // 注意:当创建了子进程之后，一定要记得进程等待，避免产生僵死进程。
  // 子进程1:
  int pid1 = fork();

  // 子进程2:
  int pid2 = fork();

  if(pid1 < 0) {

    // 进程创建失败
    perror("child1 error");
    return -1;
  }
  else if(pid1 == 0) {

    // 关闭读取端
    close(pipefd[0]);
    // 对数据进行重定向,让流入标准输出的数据流入管道的输入端。
    
    dup2(pipefd[1],1);
    // 子进程1 ---> 程序替换实现功能 ps -ef 功能。
    execlp("ps","ps","-ef",NULL);
    exit(0);
  }

  if(pid2 < 0) {

    // 进程创建失败
    perror("child2 error");
    return -1;
  }
  else if(pid2 == 0) {


    sleep(1);
    // 关闭管道的写入端
    close(pipefd[1]);
    // 对数据进行重定向，让从标准输入读取的数据，从管道的读取端进行读取。
    dup2(pipefd[0],0);

    // 子进程2 ---> 程序替换实现功能
    execlp("grep","grep","pipe_op",NULL);
    exit(0);
  }

  // 父进程都关闭
  close(pipefd[0]);
  close(pipefd[1]);
  // 进程等待
  waitpid(pid1, NULL, 0);
  printf("子进程1退出！\n");
  waitpid(pid2, NULL, 0);
  printf("子进程2退出！\n");

  return 0;
}
