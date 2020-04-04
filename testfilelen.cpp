// 使用 lseek 函数获取文件大小
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
using namespace std;

int main()
{
    int ret = open("./hello.txt", O_CREAT | O_RDWR, 0664);
    if(ret < 0) exit(-1);

    int len = lseek(ret, 0, SEEK_END);
    cout << "文件大小：" << len << endl;
    return 0;
}
