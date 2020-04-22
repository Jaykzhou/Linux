// 循环回收子线程
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sstream>
#include <string.h>
using namespace std;

void* pthread_handler(void* arg)
{
    int i = (int)arg;                   // 64 位下，会发生截断，但是没事。
    cout << "in pthread, this is " << i << "th" << endl;

    stringstream sstream;
    sstream << "current pthread is " << i << "th" << endl;
    char* buf = (char*)malloc(sizeof(char) * 100);
    memcpy(buf, sstream.str().c_str(), sstream.str().size());
    
    return (void*)buf;
}

int main()
{
    pthread_t tid[4];
    for(int i = 0; i < 4; ++i)
    {
        pthread_create(&tid[i], NULL, pthread_handler, (void*)i);
    }

    // 循环回收子线程
    for(int i = 0; i < 4; i++)
    {
        char* buf[1];
        pthread_join(tid[i], (void**)buf);
        cout << *buf;

        free(buf[0]);
    }

    return 0;
}
