
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
    //用法说明
    if(argc != 3)
    {
        printf("Usage:%s [ip][port]\n",argv[0]);
        return 1;
    }

    //打开网卡文件
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

    //向服务器端发送连接请求
    if(connect(sock,(struct sockaddr*)&server,len) < 0)
    {

        printf("connect failed\n");
        return 3;
    }

    printf("connect success\n");

    //连接成功后双方开始相互通信
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
            //当客户端发送quit时表明要与服务器端断开链接
            if(strcmp(buf,"quit") == 0)
            {
                printf("client quit\n");
                break;
            }
            //向服务器端发送消息
            write(sock,buf,strlen(buf));
            //printf("waiting ...\n");
        }

//        //从服务器端接受消息
//        buf[0] = 0;
//        ssize_t ss = read(sock,buf,sizeof(buf) - 1);
//        if(ss > 0)
//        {
//            buf[ss] = 0; 
//            printf("server say:%s\n",buf);
//        }
//    //    printf("nkxs\n");
    }

    //当客户端断开连接后，关闭客户端的文件描述符
   // close(sock);
        sleep(8);
        return 0;
}
