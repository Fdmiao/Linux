
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("Usage:%s [ip][port]\n",argv[0]);
        return 1;
    }
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        printf("socket error\n");
        return 2;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    socklen_t len = sizeof(server);

    if(connect(sock,(struct sockaddr*)&server,len) < 0)
    {

        printf("connect failed\n");
        return 3;
    }

    printf("connect success\n");

    char buf[128];
    while(1)
    {
        buf[0] = 0;
        printf("please enter#");
        fflush(stdout);
        ssize_t s = read(0,buf,sizeof(buf) - 1);
        if(s > 0)
        {
            buf[s - 1] = 0;//去掉\n，如果不去掉，在与quit比较时，quit需加上\n
            if(strcmp(buf,"quit") == 0)
            {
                printf("client quit\n");
                break;
            }
            write(sock,buf,strlen(buf));
            printf("waiting ...\n");
        }

        buf[0] = 0;
        ssize_t ss = read(sock,buf,sizeof(buf) - 1);
        if(ss > 0)
        {
            buf[ss] = 0;
            if(strcmp(buf,"quit") == 0)
            {
                printf("server not connect\n");
                break;
            }
            printf("server say:%s\n",buf);
        }
        
    }
    close(sock);
    return 0;
}
