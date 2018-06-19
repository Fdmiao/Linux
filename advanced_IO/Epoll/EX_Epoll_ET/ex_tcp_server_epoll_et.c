#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<sys/errno.h>

#define MAXSIZE 1024
typedef struct epdata
{
    int fd;//文件描述符
    int cap;//缓冲区buf的最大容量
    int size;//缓冲区buf的当前容量
    char buf[0];//柔性数组，充当缓冲区
}epdata_t,*epdata_p;


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

static void *buyEpdata(int fd,int num)
{
    epdata_p ptr = (epdata_p)malloc(sizeof(epdata_p))+num*sizeof(char);
    if(!ptr)
    {
        perror("malloc");
    }
    else
    {
        ptr->fd = fd;
        ptr->cap = num;
        ptr->size = 0;
    }
    return ptr;
}

void SetNoBlock(int fd)
{
    int flag = fcntl(fd,F_GETFL);
    if(flag < 0)
    {
        perror("flag error");
        return;
    }
    fcntl(fd,F_SETFL,flag|O_NONBLOCK);
    return;
}
//循环处理接受连接事件
void accept_server(int epfd,int listen_socket)
{
    while(1)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        //循环式的读
        int connfd = accept(listen_socket,(struct sockaddr*)&client,&len);
        if(connfd < 0)
        {
            //printf("accept error\n");
            break;
        }
        SetNoBlock(connfd);
        printf("get a new connect:[%s][%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
        struct epoll_event con_ev;
        con_ev.events = EPOLLIN | EPOLLET;
        //修改的地方
        con_ev.data.ptr = buyEpdata(connfd,1024*5);
        epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&con_ev);
    }
    return;
}

int NoBlockRead(int fd,char* buf,int size)
{
    int total = 0;
    while(1)
    {
        int cur_size = read(fd,buf + total,size - total);
//        int cur_size = read(fd,buf + total,20);
        if(cur_size > 0)
        {
            total = total + cur_size;
        }
        else if(total >= size || errno == EAGAIN)
        {
            break;
        }
    }
    return total;
}
//此时为普通文件描述符读就绪
//需要修改的地方
void read_server(int epfd,char* ptr)
{
 //   注意修改之后的值
    char* buf = ((epdata_p)ptr)->buf;
    int size = ((epdata_p)ptr)->size;
    int cap = ((epdata_p)ptr)->cap;
    int fd = ((epdata_p)ptr)->fd;
    ssize_t s = NoBlockRead(fd,buf + size,cap - size - 1);
    //ssize_t s = NoBlockRead(fd,buf,cap);
    if(s > 0)
    {
        ((epdata_p)ptr)->size += s;
        //考虑协议方面的内容，是否读到空行等
        buf[s] = 0;
        printf("client %d:%s\n",fd,buf);
        //修改的地方
        struct epoll_event ev;
        ev.events = EPOLLOUT;
        ev.data.ptr = (epdata_p)ptr;
        epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);
    }
    else if(s == 0)
    {
        printf("client %d quit\n",fd);
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
        close(fd);
        free(ptr);
    }
    else
    {
        printf("read error\n");
        //修改的地方
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
        close(fd);
        free(ptr);
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
        //注意修改之后的值
        int fd = ((epdata_p)(wait_ev[index].data.ptr))->fd;

        char* buf = (char*)(wait_ev[index].data.ptr);
        //如果有读事件就绪
        if(wait_ev[index].events & EPOLLIN)
        {
            //此时为监听套接字就绪
            if(fd == listen_socket)
            {
                //循环式的接受新链接
                accept_server(epfd,listen_socket);
            }
            else
            {
                //此时为普通文件描述符读就绪
                //需要修改的地方
                read_server(epfd,buf);
            }
        }
        //可能有非读事件就绪，这里只处理读事件，所以当其他时间就绪时，不进行处理
        //修改的地方
        //写事件就绪
        else if(wait_ev[index].events & EPOLLOUT)
        {
            //将读到的消息回显回去
            const char* msg = "HTTP/1.0 200 OK\r\n\r\n<html><h1>EPOLL SUCCESS:)</h1><html>\r\n";
            write(fd,msg,strlen(msg));
            epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
            close(fd);
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


    //获得监听套接字
    int listen_fd = listen_server(atoi(argv[1]));
    printf("bind and listen success,....\n");

    //将监听文件描述符设置为非阻塞
    SetNoBlock(listen_fd);

    //创建epoll模型
    int epfd = epoll_create(50);
    if(epfd < 0)
    {
        perror("epoll_create");
        return 5;
    }


    //将监听套接字注册进epoll模型中
    struct epoll_event lis_ev;
    lis_ev.events = EPOLLIN | EPOLLET;//添加读事件
    //修改的地方
    lis_ev.data.ptr = buyEpdata(listen_fd,1);
    int lis_ret = epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&lis_ev);
    if(lis_ret < 0)
    {
        perror("ctl listen_fd error");
        return 6;
    }
    //进行循环式的等待
    while(1)
    {
        //定义一个数，接收就绪的事件
        struct epoll_event event_list[MAXSIZE];
        int ret = epoll_wait(epfd,event_list,sizeof(event_list)/sizeof(event_list[0]),-1);
        if(ret < 0)
        {
            perror("wait error");
            continue;
        }
        else if(ret == 0)
        {
            printf("timeout\n");
        }
        else
        {
            server(epfd,event_list,ret,listen_fd);
        }
    }
}
