// 使用消息队列完成进程间网状通信
// 当前程序完成从消息队列中读取数据
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
using namespace std;

#define FILEQUEUE "./filequeue.txt"
#define ID 'a'

// 封装消息包
struct Message
{
    long type;
    char data[100];
};

// 消息队列标识符
int msgid = 0;

void rmmsgqueue(int signo)
{
    msgctl(msgid, IPC_RMID, NULL);
    cout << "删除消息队列完成！" << endl;
    exit(-1);
}

int main()
{
    // 捕捉 ctrl + c 信号
    signal(SIGINT, rmmsgqueue);

    // 使用文件名和项目ID生成唯一IPC标识符
    int ipc = ftok(FILEQUEUE, ID);

    // 将唯一标识符传入到消息队列中
    msgid = msgget(ipc, IPC_CREAT | IPC_EXCL | 0664);
    if(msgid < 0)
    {
        perror("msgget error!");
        exit(-1);
    }

    // 循环收发消息
    while(1)
    {
        long type = 0;
        struct Message data;
        cout << "输入你想获取的消息类型: ";
        cin >> type;
        int ret = msgrcv(msgid, &data, sizeof(data), type, MSG_NOERROR);
        if(ret < 0)
        {
            perror("msgrcv error!");
            exit(-1);
        }
        cout << "接受到的消息为: " << data.data << endl;

        struct Message snddata;
        snddata.type = 22;
        char* buf = "我收到了你的消息了！";
        strcpy(snddata.data, buf);
        msgsnd(msgid, &snddata, sizeof(snddata), 0);
    }

    return 0;
}
