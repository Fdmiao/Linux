
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

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
        perror("socket error");
        return 2;
    }
    
    char buf[128];
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t len = sizeof(server);
    
    struct sockaddr_in peer;
    socklen_t len1 = sizeof(peer);

    while(1)
    {
        buf[0] = 0;
        printf("please enter#");
        fflush(stdout);
        ssize_t s = read(0,buf,sizeof(buf) - 1);
        if(s > 0)
        {
            buf[s - 1] = 0;//会把回车读入
            sendto(sock,buf,strlen(buf),0,(struct sockaddr*)&server,len);
        }

        size_t ss = recvfrom(sock,buf,sizeof(buf) - 1,0,(struct sockaddr*)&peer,&len1);
        if(ss > 0)
        {
            buf[ss] = 0;
            printf("server# %s\n",buf);
        }
    }
    close(sock);
    return 0;
}
