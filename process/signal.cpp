// 练习信号自定义处理方式
#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

void handle(int signo)
{
    if(signo == SIGINT)
    {
        cout << "recv SIGINT!" << endl;
    }
}

int main()
{
    //自定义处理函数
    signal(SIGINT, handle);
    while(1);

    return 0;
}
