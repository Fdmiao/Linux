#include "comm.h"

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
    int ret = 0;
    char arr[ROW][COL] = { 0 };
    Init(arr, ROW, COL);//初始化数组
    print(arr,ROW,COL);
    while (1)
    {
        printf("please wait...\n");
        location client_loc;
        read(client_sock,&client_loc,sizeof(client_loc));//客户端先走
        arr[client_loc.x - 1][client_loc.y - 1] ='*';
        print(arr,ROW,COL);//打印棋盘
        ret = iswin(arr,ROW,COL);//判断是否有人胜利
        int r = who_win(ret); 
        if(r == 1)
        {
            //有人赢
            break;
        }

        location loc;
        printf("玩家1走，玩家1请选择：");        
        move(arr,ROW,COL,&loc,'Y');//客户端移动
        print(arr, ROW, COL);//打印棋盘
        write(client_sock,&loc,sizeof(loc));
        ret = iswin(arr,ROW,COL);//判断是否有人胜利
        r = who_win(ret); 
        if(r == 1)
        {
            //有人赢
            break;
        }
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
            while(1)
            {
                Response res;
                read(client_sock,&res,sizeof(res));
                if(res.res == 0)
                {
                    break;
                }
                printf("开始游戏\n");
                server_work(client_sock,ip_buf,port);
            }
            printf("client quit\n");
            exit(0);
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
    }

    return 0;
}
