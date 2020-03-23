#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include "wrap.h"
using namespace std;

int main()
{
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7777);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    while(1)
    {
        char buf[1024];
        bzero(buf, 1024);
        cin >> buf;
        struct sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(8888);
        inet_pton(AF_INET, "192.168.248.129", (void*)&saddr.sin_addr);
        sendto(sockfd,  buf, 1024, 0, (struct sockaddr*)&saddr, sizeof(saddr));

        bzero(buf, 1024);
        recvfrom(sockfd, buf, 1024, 0, NULL, NULL);
        cout << buf << endl;
    }

    return 0;
}
