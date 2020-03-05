// 可重入函数与不可重入函数
#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

void Reentrantfun()
{
    int i = 0, count = 0;
    while(count < 10000)
    {
        cout << i << endl;
        i++;
        count++;
        usleep(100);
    }
}

int i = 0;
void Unreentrantfun()
{
    int count = 0;
    while(count < 10000)
    {
        cout << i << endl;
        i++;
        count++;
        usleep(100);
    }
}

void handle(int signo)
{
    i += 10;
    cout << "inter funaction!" << endl;
}

int main()
{
    signal(SIGINT, handle);

    // 先测试可重入函数
    // Reentrantfun();

    // 测试不可重入函数
    Unreentrantfun();

    return 0;
}
