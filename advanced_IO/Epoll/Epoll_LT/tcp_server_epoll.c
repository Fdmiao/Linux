

//利用epoll实现多路转接
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#define MAXSIZE 1024//设置所关心的事件最多为1024


//获取监听套接字
int listen_server(int port)
{
    //打开网卡，创建文件描述符
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("sock");
        return 2;
    }

    //可以绑定IP地址不同但端口号相同的套接字
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = (htonl)INADDR_ANY;
    local.sin_port = htons(port);
    //绑定套接字
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        perror("bind");
        return 3;
    }

    //监听套接字
    if(listen(sock,5) < 0)
    {
        perror("listen error");
        return 4;
    }
    return sock;

}

//处理接受连接事件
void accept_server(int epfd,int listen_socket)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int connfd = accept(listen_socket,(struct sockaddr*)&client,&len);
    if(connfd < 0)
    {
        printf("accept error\n");
        return;
    }
    printf("get a new connect:[%s][%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    struct epoll_event con_ev;
    con_ev.events = EPOLLIN;
    con_ev.data.fd = connfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&con_ev);
}
//此时为普通文件描述符读就绪
void read_server(int epfd,int fd)
{
    char buf[1024] = {0};
    ssize_t s = read(fd,buf,sizeof(buf) - 1);
    if(s > 0)
    {
        buf[s] = 0;
        printf("client %d:%s\n",fd,buf);
    }
    else if(s == 0)
    {
        printf("client %d quit\n",fd);
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
        close(fd);
    }
    else
    {
        printf("read error\n");
    }
    return;
}
//此时有事件就绪，需处理就绪事件,size为就绪事件的个数
void server(int epfd,struct epoll_event* wait_ev,int size,int listen_socket)
{
    //遍历数组，处理就绪事件
    int index = 0;
    for(;index < size;index++)
    {
        //如果有读事件就绪
        if(wait_ev[index].events & EPOLLIN)
        {
            //此时为监听套接字就绪
            if(wait_ev[index].data.fd == listen_socket)
            {
                accept_server(epfd,listen_socket);
            }
            else
            {
                //此时为普通文件描述符读就绪
                read_server(epfd,wait_ev[index].data.fd);
            }
        }
        //可能有非读事件就绪，这里只处理读事件，所以当其他时间就绪时，不进行处理
        else
        {
            continue;
        }
    }
}
int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        printf("Usage:%s [port]\n",argv[0]);
        return 1;
    }

    //获取监听套接字
    int listen_socket = listen_server(atoi(argv[1]));
    printf("bind and listen success\n");

    //创建epoll模型
    int epfd = epoll_create(50);
    if(epfd < 0)
    {
        perror("epoll");
        return 5;
    }

    //注册监听事件

    struct epoll_event lis_ev;
    lis_ev.events = EPOLLIN;//只关注读事件
    //申请一片空间存放接收到的内容
    //buy_buf* tmp = Buy_Buffer(listen_socket);
    //lis_ev.data.ptr = tmp;
    lis_ev.data.fd = listen_socket;

    //将监听套接字注册进epoll模型
    int lis_ret = epoll_ctl(epfd,EPOLL_CTL_ADD,listen_socket,&lis_ev);
    //这里应该判断下，因为监听描述符如果没有添加进去，后续的操作都不能进行了
    if(lis_ret < 0)
    {
        perror("ctl listen error");
        return 6;
    }

    //循环式的等待就绪事件
    while(1)
    {
        struct epoll_event wait_ev[MAXSIZE];
        int ret = epoll_wait(epfd,wait_ev,MAXSIZE,-1);
        //有事件就绪
        if(ret > 0)
        {
            server(epfd,wait_ev,ret,listen_socket);
        }
        else if(ret == 0)
        {
            printf("timeout\n");
        }
        else
        {
            printf("epoll_wait error\n");
        }
    }
    return 0;
}


