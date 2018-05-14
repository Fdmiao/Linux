
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>

int listen_work(int sock,int ip,int port)
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = ip;
    server.sin_port = port;

    //绑定套接字
    if(bind(sock,(struct sockaddr*)&server,sizeof(server)) < 0)
    {
        printf("bind error\n");
        exit(3);
    }
    //将套接字设置为监听套接字
    if(listen(sock,5) < 0)
    {
        printf("listen error\n");
        exit(4);
    }
    return sock;
}

void server_work(int client_sock)
{
    while(1)
    {
        char client_buf[1024];
        client_buf[0] = 0;
        ssize_t s = read(client_sock,client_buf,sizeof(client_buf));
        if(s > 0)
        {
            client_buf[s] = 0;
            printf("%s\n",client_buf);
        }

        const char* buf = "<h1>hello</h1>";
        char server_buf[1024];
        sprintf(server_buf,"HTTP/1.0 200 OK\nContent-Length:%d\n\n%s\n",strlen(buf),buf);
        write(client_sock,server_buf,strlen(server_buf));
    }

}

void process_work(int client_sock,int listen_sock)
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        printf("fork error\n");
        return;
    }
    else if(pid == 0)
    {
        close(listen_sock);
        pid_t pid1 = fork();
        if(pid1 < 0)
        {
            printf("fork1 error\n");
            return;
        }
        else if(pid1 == 0)
        {
            server_work(client_sock);
            close(client_sock);
            exit(0);
        }
        else 
        {
            exit(0);
        }
    }
    else
    {
        close(client_sock);
        waitpid(pid,NULL,0);
    }
}

int main(int argc,char*argv[])
{
    if(argc != 3)
    {
        printf("Usage:%s[ip][port]\n",argv[0]);
        return 1;
    }
    //打开网卡/套接字文件
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        printf("socket error\n");
        return 2;
    }

    //将套接字设置为监听套接字
    int listen_sock = listen_work(sock,inet_addr(argv[1]),htons(atoi(argv[2])));
    printf("bind and listen success,wait connect...\n");

    daemon(0,0);//创建守护进程

    struct sockaddr_in client;
    while(1)
    {
        int client_len = sizeof(client);

        int client_sock = accept(listen_sock,(struct sockaddr*)&client,&client_len);
        if(client_sock < 0)
        {
            printf("accept error\n");
            continue;
        }

        process_work(client_sock,listen_sock); 
        //server_work(client_sock);
    }
    return 0;
}
