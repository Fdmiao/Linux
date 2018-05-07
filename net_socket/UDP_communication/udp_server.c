

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
        printf("Usage:%s [ip] [port]\n",argv[0]);
        return 1;
    }

    //打开网卡，得到文件描述符
    //参数1为套接字/地址类型，参数2为服务类型
    int sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0)
    {
        perror("socket error");
        return 2;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));
    local.sin_addr.s_addr =inet_addr(argv[1]);

    //绑定端口号: 提供服务时，要找到服务器，必须有ip地址和端口号
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        perror("bind error");
        return 3;
    }

    char buf[128];
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    while(1)
    {
        buf[0] = 0;
        size_t s = recvfrom(sock,buf,sizeof(buf) - 1,0,(struct sockaddr*)&client,&len);
        if(s > 0)
        {
            buf[s] = 0;
            printf("[%s][%d]:%s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),buf);
        }

        sendto(sock,buf,strlen(buf),0,(struct sockaddr*)&client,len);

    }
    

    close(sock);
    return 0;
}
