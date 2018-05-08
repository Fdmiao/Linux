#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

int server_sock(char* ip,int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        printf("socker error\n");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    socklen_t len = sizeof(server);
    if(bind(sock,(struct sockaddr*)&server,len) < 0)
    {
        printf("bind error\n");
        exit(2);
    }

    if(listen(sock,10) < 0)
    {
        printf("listen error\n");
        exit(3);
    }

    return sock;//得到监听套接字
}

void server_work(int client_sock,char* ip,int port)
{
    char buf[128];
    while(1)
    {
        buf[0] = 0;
        ssize_t s = read(client_sock,buf,sizeof(buf) - 1);
        if(s > 0)
        {

            buf[s] = 0;
            printf("ip:%s,port:%d say# %s\n",ip,port,buf);
        }
        else if(s == 0)
        {
            printf("ip:%s,port:%d quit\n",ip,port);
            break;
        }
        else
        {
            printf("read error\n");
            break;
        }

        printf("please enter#");
        fflush(stdout);
        buf[0] = 0;
        int ss = read(0,buf,sizeof(buf) - 1);
        if(ss > 0)
        {
            buf[ss - 1] = 0; 
        }

        write(client_sock,buf,strlen(buf));
        printf("waiting ...\n");
    }
}
void process_work(int client_sock,char* ip_buf,int port)
{
    pid_t pid = fork();
    if(pid < 0)
    {
        printf("fork error\n");
        return;
    }
    else if(pid == 0)//child
    {
        pid_t pid1 = fork();
        if(pid1 < 0)
        {
            printf("fork error\n");
            return;
        }
        else if(pid1 == 0)//grandchild
        {
            server_work(client_sock,ip_buf,port);
        }
        exit(0);
    }
    else//father
    {
        waitpid(pid,NULL,0);
    }
}
int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("Usage:%s [ip][port]\n",argv[0]);
        return 1;
    }
    
    int listen_sock = server_sock(argv[1],atoi(argv[2]));//得到监听套接字
    printf("bind and listen success,wait accept...\n");


    struct sockaddr_in client;

    while(1)
    {
        socklen_t len = sizeof(client);
        int client_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
        if(client_sock < 0)
        {
            printf("accept error\n");
            continue;
        }

        char ip_buf[24];
        ip_buf[0] = 0;
        inet_ntop(AF_INET,&client.sin_addr,ip_buf,sizeof(ip_buf));
        int port = ntohs(client.sin_port);
        printf("connect success, ip:[%s],port:[%d]\n",ip_buf,port);

        process_work(client_sock,ip_buf,port);
        close(client_sock);


    }

    return 0;
}
