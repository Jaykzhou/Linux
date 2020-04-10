// 使用消息队列进行进程间通信(网状)
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sstream>
#include <string>   
using namespace std;

#define FILEQUEUE "./filequeue.txt"
#define ID 'a'

// 封装消息包
struct Message
{
    long type;
    char data[50];
};

int main()
{
    // 使用文件名以及项目ID生成唯一的IPC标识符
    int ipc = ftok(FILEQUEUE, ID);
    if(ipc < 0)
    {
        perror("ftok error!");
        exit(-1);
    }

    // 利用唯一标识符加入到消息队列中
    int msgid = msgget(ipc, 0664);
    if(msgid < 0)
    {
        perror("msgget error!");
        exit(-1);
    }

    // 循环发送消息
    int count = 0;
    while(count++ < 50)
    {
        char* buf = "今天是不错的一天！";
        stringstream ss;
        ss << "当前是第 " << count << " 个数据，该数据为: " << buf; 
        struct Message data;
        data.type = count;
        strcpy(data.data, ss.str().c_str());
        cout << data.data << endl;
        msgsnd(msgid, &data, sizeof(data), 0);
    }

    cout << "消息发送完成！" << endl;

    return 0;
}
