#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

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
        printf("sock error\n");
        return 2;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    socklen_t len = sizeof(server);

    char buf[128];
    struct sockaddr_in peer;
    while(1)
    {
        socklen_t len1 = sizeof(peer);
        printf("please enter#");
        fflush(stdout);
        buf[0] = 0;
        ssize_t s = read(0,buf,sizeof(buf) - 1);
        if(s > 0)
        {
            buf[s - 1] = 0;
            sendto(sock,buf,strlen(buf),0,(struct sockaddr*)&server,len);
        }
        else if(s < 0)
        {
            break;
        }

        ssize_t ss = recvfrom(sock,buf,sizeof(buf) - 1,0,(struct sockaddr*)&peer,&len1);
        if(ss > 0)
        {
            buf[ss] = 0;
            printf("server# %s\n",buf);
        }
    }

    close(sock);
    return 0;
}
