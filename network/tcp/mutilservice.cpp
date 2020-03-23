#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>
#include <ctype.h>
#include "wrap.h"
using namespace std;

#define PROT 8888
#define ADDR "192.168.248.129"

void waitchild(int signo)
{
    if(signo == SIGCHLD)
        while(waitpid(-1, NULL, WNOHANG) > 0);

    return;
}

int main()
{
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PROT);
    inet_pton(AF_INET, ADDR, (void*)&saddr.sin_addr);
    Bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));

    Listen(lfd, 128);
    
    while(1)
    {
        struct sockaddr_in caddr;
        socklen_t lenth = sizeof(caddr);
        int cfd = Accept(lfd, (struct sockaddr*)&caddr, &lenth);

        pid_t pid = fork();
        if(pid < 0)
        {
            perror("fork error");
            exit(-1);
        }
        else if(pid == 0)
        {
            close(lfd);
            while(1)
            {
                char buf[1024];
                bzero(buf, 1024);
                int ret = Read(cfd, buf, 1024);
                if(ret == 0) break;
                cout << buf;

                for(int i = 0; i < ret; ++i)
                    buf[i] = toupper(buf[i]);
                Write(cfd, buf, ret);
            }

            close(cfd);
            return 0;
        }
        else 
        {
            close(cfd);
            signal(SIGCHLD, waitchild);
        }
    }

    return 0;
}
