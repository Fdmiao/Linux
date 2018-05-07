#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("Usage:%s [ip][port]\n",argv[0]);
        return 1;
    }

    int sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0)
    {
        printf("socket error\n");
        return 2;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = inet_addr(argv[1]);
    local.sin_port = htons(atoi(argv[2]));
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        printf("bind error\n");
        return 3;
    }

    char buf[128];
    struct sockaddr_in client;
    while(1)
    {
        socklen_t len = sizeof(client);//每次接受到得客户端可能不同，所以，要在循环中求长度
        ssize_t s = recvfrom(sock,buf,sizeof(buf) - 1,0,(struct sockaddr*)&client,&len);
        if(s > 0)
        {
            buf[s] = 0;
            printf("%s %d say# %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),buf);
        }
        sendto(sock,buf,strlen(buf),0,(struct sockaddr*)&client,len);

    }
    return 0;
}
