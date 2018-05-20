
#include"comm.h"

int server_sock(char* ip,int port)
{
    //打开网卡，获得文件描述符
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)//打开失败
    {
        printf("socker error\n");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    socklen_t len = sizeof(server);
    //将服务器进程绑定特定的端口号
    if(bind(sock,(struct sockaddr*)&server,len) < 0)//绑定失败
    {
        printf("bind error\n");
        exit(2);
    }

    //设置监听套接字
    if(listen(sock,10) < 0)//监听失败
    {
        printf("listen error\n");
        exit(3);
    }

    return sock;//返回得到的监听套接字
}

void server_work(int client_sock,char* ip,int port)
{

    //该部分内容用于得到浏览器的请求报文
    //在该服务器按绑定的IP地址和端口号运行起来后
    //在浏览器上根据HTTP协议访问上述的的IP地址和端口号即可得到对应的HTTP协议报文
    char buf[10240];

    read(client_sock,buf,sizeof(buf));
    printf("%s",buf);
    printf("=========================\n");

    //以下注释部分用于实现网络版的加法计算器
    //Request req;//定义结构体变量用于接受客户端发来的两个加数
    //Response res;//定义结构体变量用于存储计算的结果
    //while(1)
    //{
    //    char buf[10240];

    //    read(client_sock,buf,sizeof(buf));
    //    printf("%s",buf);
    //    printf("=========================\n");
    //    ////从客户端读取两个加数
        //ssize_t s = read(client_sock,&req,sizeof(req));
        //if(s < 0)//读取失败
        //{
        //    //错误处理
        //    printf("read error\n");
        //    break;
        //}
        //else if(s == 0)
        //{
        //    ///客户端已断开连接
        //    printf("client:[%s][%d] quit\n",ip,port);
        //    break;
        //}
        ////计算结果
        //res.sum = req.x + req.y;

        ////将结果发送给客户端
        //write(client_sock,&res,sizeof(res));
//    }
}
void process_work(int client_sock,int listen_sock,char* ip_buf,int port)
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
            close(listen_sock);//关闭不用的文件描述符
            server_work(client_sock,ip_buf,port);
        }
        close(client_sock);//关闭不用的文件描述符
        exit(0);
    }
    else//father
    {
        close(client_sock);//关闭不用的文件描述符
        waitpid(pid,NULL,0);
    }
}
int main(int argc,char* argv[])
{
    if(argc != 3)//用法说明
    {
        printf("Usage:%s [ip][port]\n",argv[0]);
        return 1;
    }
    
    //获得监听套接字（该文件描述符用于处理请求连接）
    int listen_sock = server_sock(argv[1],atoi(argv[2]));//得到监听套接字
    printf("bind and listen success,wait accept...\n");


    struct sockaddr_in client;

    while(1)
    {
        socklen_t len = sizeof(client);
        //接受来自客户端发来的请求连接，得到用于服务的文件描述符
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

        process_work(client_sock,listen_sock,ip_buf,port);
        close(client_sock);


    }

    return 0;
}
