#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<sys/errno.h>

#define MAX 128

//将某一文件描述符的相关时间信息封装在下述结构体中
typedef struct epdata
{
    int fd;//文件描述符
    int size;//缓冲区中已有数据的大小
    int cap;//缓冲区的总大小
    char buf[0];//柔性数组充当缓冲区
}epdata_t,*epdata_p;

//获得监听套接字函数
int startup(int port)
{
    //获得监听套接字
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("sock");
        exit(2);
    }

    //将监听套接字设置为可以绑定不同IP地址的相同端口号
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(port);

    //绑定套接字
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        perror("bind error");
        exit(3);
    }

    //监听套接字
    if(listen(sock,5) < 0)
    {
        perror("listen,error");
        exit(4);
    }

    return sock;

}
//将文件描述符设置为非阻塞
void SetNoBlock(int fd)
{
    int flag = fcntl(fd,F_GETFL);
    if(flag < 0)
    {
        perror("fcntl error");
        return;
    }
    //将文件描述符设置为非阻塞
    fcntl(fd,F_SETFL,O_NONBLOCK);
}

//当新申请一个文件描述符是，为将该文件描述符所读到的内容存储起来
//需要开辟一段缓冲区来存放读到的数据
//该函数相当于初始化缓冲区
epdata_p GetBuff(int fd,int size)
{
    //申请一片区域：包含文件描述符和缓冲区
    epdata_p ptr = (epdata_p)malloc(sizeof(epdata_t) + size*sizeof(char));
    ptr->fd = fd;
    ptr->size = 0;//初始时缓冲区中没有数据，所以该字段大小为0
    ptr->cap = size;//缓冲区的总大小设置为size
    return ptr;
}
//循环接受连接
void accept_server(int epfd,int listen_fd)
{
    while(1)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int connfd = accept(listen_fd,(struct sockaddr*)&client,&len);
        //连接接受完毕
        if(connfd < 0)
        {
            break;
        }
        else if(connfd > 0)//获取到一个新连接
        {
            printf("get a new coonect:[%s][%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
            //将新连接设置为非阻塞方式
            SetNoBlock(connfd);
            //创建新文件描述符的事件结构体
            struct epoll_event connfd_ev;
            connfd_ev.events = EPOLLIN | EPOLLET;
            connfd_ev.data.ptr = GetBuff(connfd,1024*5);

            //将新连接注册进epoll模型
            int ret = epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&connfd_ev);
            if(ret < 0)
            {
                perror("epoll_ctl error");
                continue;
            }
        }
    }
}
int ReadNoBlock(int fd,char* buf,int size)
{
    int total = 0;
    while(1)
    {
        int s = read(fd,buf + total,size - total);
        if(s > 0)
        {
            total = total + s;
        }
        else if(s == 0)
        {
            printf("client %d quit\n",fd);
            break;
        }
        else if(total >= size || errno == EAGAIN)
        {
            break;
        }
    }
    return total;
}
//处理普通文件描述符的读事件
void read_server(int epfd,epdata_p ptr,int fd)
{
    //首先要进行非阻塞式的读，将本次发送的数据一次性读完
    //将读取的内容放到缓冲区中，缓冲区中可能有之前读到的数据
    //所以，本次读到的数据放在之前数据之后，组成完成的数据报文
    //因此，需要知道缓冲区中已有数据的多少，及缓冲区的总大小
    //所以，需要在申请的结构体中在定义两个变量，用于标识缓冲区的总大小和已有数据的大小

    char* buf = ptr->buf;
    int size = ptr->size;
    int cap = ptr->cap;
    int s = ReadNoBlock(fd,buf + size,cap - size -1);
    if(s > 0)
    {
        ptr->size += s;
        buf[ptr->size] = 0;
        printf("client:%d %s\n",fd,buf);
        //读完之后，将事件设置为写事件
        struct epoll_event fd_ev;
        fd_ev.events = EPOLLOUT;
        fd_ev.data.ptr = ptr;
        //将该文件描述符进行注册修改
        epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&fd_ev);
    }
    else if(s == 0)
    {
        printf("client %d quit\n",fd);
        close(fd);
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
        free(ptr);
    }
    else
    {
        printf("read error\n");
        close(fd);
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
        free(ptr);
    }
}
//处理写事件
void write_service(int epfd,epdata_p ptr,int fd)
{
    char* msg = "HTTP/1.0 OK 200\r\n\r\n<html><h1>SUCCESS:)</h1></html>\r\n";
    write(fd,msg,strlen(msg));
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
    close(fd);
}
//有事件就绪，接下来开始进行处理就绪事件
//epfd为epoll模型的文件描述符，event_list为就绪事件数组的指针，num的就绪事件的个数
void server(int epfd,struct epoll_event* event_list,int num,int listen_fd)
{
    //首先要遍历数组判断每个数组元素所代表的文件描述符是读事件就绪还是写事件就绪
    int index = 0;
    for(;index < num;index++)
    {
        int fd = ((epdata_p)(event_list[index].data.ptr))->fd;
        epdata_p ptr = (epdata_p)event_list[index].data.ptr;
        //读事件就绪
        if(event_list[index].events & EPOLLIN)
        {
            //如果是读事件就绪，再判断是监听套接字就绪，还是普通文件描述符就绪
            //如果是监听套接字就绪
            if(fd == listen_fd)
            {
                //循环接受连接
                accept_server(epfd,listen_fd);
            }
            else//普通文件描述符就绪
            {
                //处理普通文件描述符的读事件
                read_server(epfd,ptr,fd);
            }
        }
        //写事件就绪
        if(event_list[index].events & EPOLLOUT)
        {
            //处理写事件
            write_service(epfd,ptr,fd);
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
    int listen_fd = startup(atoi(argv[1]));
    printf("bind and listen success...");


    //本程序采用的是边缘触发方式进行读写
    //在将监听套接字注册进epoll模型之前
    //因为可能同时会到达很多连接，可能监听套接字一次处理不完，
    //但是又因为是边缘触发方式，当就绪事件发生时，必须一次读完
    //所以要将监听套接字设置为非阻塞方式，进行循环式的接受连接，直到将就绪的连接接受完毕为止
    
    //将监听套接字设置为非阻塞
    SetNoBlock(listen_fd);

    //接下来：
    //首先要创建epoll模型，获取epoll模型的文件描述符
    int epfd = epoll_create(250);
    if(epfd < 0)//模型创建失败，直接返回
    {
        perror("epoll_create error");
        return 6;
    }

    struct epoll_event lis_ev;//创建监听文件描述符的事件结构体
    lis_ev.events = EPOLLIN | EPOLLET;//监听套接字只关心读事件，并且将其设置为边缘触发方式

    //给监听套接字分配一片缓冲区，并将该缓冲区设置进epoll模型中
    //该缓冲区中包含：文件描述符，
    //                一片缓冲区：用于存放多次非阻塞读取的数据，因为一次读取数据之后可能形成不了一个完整的报文
    //                            因此不能对其进行立即处理，所以现将其之前读取的内容存放起来，待整个报文读取完毕之后再进行处理
    //                            这里的缓冲区要进行动态内存申请，因为如果在栈区申请的内存，在一次非阻塞读完之后，就会自动释放
    //                            报文的后续内容到达之后就形成不了完整的报文，所以要在堆区进行申请内存，这样本次内容读取完之后
    //                            就会保存下来，和之后的内容一起形成完整的报文
    lis_ev.data.ptr = GetBuff(listen_fd,10);//给监听套接字分配一片缓冲区，将该缓冲区设置进epoll模型中

    //然后将监听套接字注册进epoll模型中
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &lis_ev);
    if(ret < 0)//如果监听套接字没有注册进epoll模型，后续的操作都将进行不了，所以，一旦出错，直接返回即可
    {
        perror("epoll_ctl error");
        return 7;
    }
    //最后开始循环式的等待就绪事件发生
    while(1)
    {
        //在等待的时候要将就绪返回的时间保存在事件数组中，所以在等待之前要创建一个事件数组
        struct epoll_event event_list[MAX];

        //以阻塞的方式进行等待
        int num = epoll_wait(epfd,event_list,MAX,-1);
        if(num < 0)//函数调用出错
        {
            perror("epoll_wait error");
        }
        else if(num == 0)//超时返回
        {
            printf("timeout\n");
        }
        else//num>0时，为就绪事件的个数
        {
            //有事件就绪，接下来开始进行处理就绪事件
            server(epfd,event_list,num,listen_fd);
        }
    }

    return 0;
}
