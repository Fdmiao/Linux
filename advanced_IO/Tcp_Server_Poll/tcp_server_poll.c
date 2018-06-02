
//使用poll实现多路转接
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/poll.h>


typedef struct pollfd pollfd;
#define MAXSIZE 1024

//得到监听套接字
int listen_service(int port)
{
    //打开网卡文件，得到文件描述符
    int sock = socket(AF_INET,SOCK_STREAM,0);
    //文件描述符获取失败
    if(sock < 0)
    {
        perror("sock error");
        exit(2);
    }

    //将文件描述符设定为可以绑定不同IP地址的相同端口号
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = (htonl)INADDR_ANY;
    local.sin_port = htons(port);
    socklen_t len = sizeof(local);
    //绑定文件描述符
    if(bind(sock,(struct sockaddr*)&local,len) < 0)
    {
        perror("bind error");
        exit(3);
    }

    //获取监听套接字
    if(listen(sock,5) < 0)
    {
        perror("listen error");
        exit(4);
    }
    //返回监听套接字
    return sock;
}
//初始环数组
void InitArr(pollfd* fd_list,int size)
{
    //遍历数组，将数组的每个元素进行初始化
    int index = 0;
    for(;index < size;index++)
    {
        fd_list[index].fd = -1;
        fd_list[index].events = 0;
        fd_list[index].revents = 0;
    }
    return;
}
//将监听套接字添加进数组中
void AddFd(pollfd* fd_list,int size,int fd,short events)
{
    //遍历数组，找到数组元素的文件描述符值为-1的元素
    //将该文件描述符及所关心的事件添加进结构体数组
    int index = 0;
    for(;index < size;index++)
    {
        if(fd_list[index].fd == -1)
        {
            fd_list[index].fd = fd;
            fd_list[index].events = events;
            return;
        }
    }
    printf("fd_list is full\n");
    return;
}

//就绪的文件描述符是监听套接字
//此时应该提供accept服务
void accept_Server(pollfd* fd_list,int size,int fd)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    int connfd = accept(fd,(struct sockaddr*)&client,&len);
    if(connfd < 0)
    {
        perror("accept");
        return;
    }
    printf("get a new connect:[%s][%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    
    AddFd(fd_list,size,connfd,POLLIN);
}
//将文件描述符从文件中删除
void DelArr(pollfd* fd_list,int size,int fd)
{
    //遍历数组，找到要删除的文件描述符
    //然后将该数组元素的相关成员置空
    int index = 0;
    for(;index < size;index++)
    {
        if(fd_list[index].fd == fd)
        {
            fd_list[index].fd = -1;
            fd_list[index].events = 0;
            fd_list[index].revents = 0;
            return;
        }
    }
    return;
}
//就绪的文件描述符是普通文件
void Read_Server(pollfd* fd_list,int size,int fd)
{
    char buf[1024] = {0};
    ssize_t s = read(fd,buf,sizeof(buf) - 1);
    if(s > 0)
    {
        buf[s - 1] = 0;
        printf("client %d:%s\n",fd,buf);
    }
    else if(s == 0)
    {
        printf("client %d quit\n",fd);
        close(fd);
        //将文件描述符从文件中删除
        DelArr(fd_list,size,fd);
    }
    else
    {
        printf("%d read error\n",fd);
        close(fd);
        DelArr(fd_list,size,fd);
    }
}
//此时必有文件描述符就绪，开始判断就绪的文件描述符
//如果是监听套接字就绪，就开始accept接受新链接
//如果是普通文件描述符就绪，就开始调用read读取数据
void Fd_Server(pollfd* fd_list,int size)
{
    //遍历数组元素，找到所关心的文件描述符
    //并且该文件描述符的revent中有读时间
    //此时该文件描述符一定就绪
    //接下来在判断该文件描述符的类型，进而提供不同的服务
    int index = 0;
    for(;index < size;index++)
    {
        if(fd_list[index].fd != -1 && (fd_list[index].revents & POLLIN))
        {
            //此时必有文件描述符就绪
            if(index == 0)
            {
                //就绪的文件描述符是监听套接字
                accept_Server(fd_list,size,fd_list[index].fd);
                continue;
            }
            //就绪的文件描述符是普通文件
            Read_Server(fd_list,size,fd_list[index].fd);
        }
    }
}
//输出所关心的文件描述符集
void CareFd(pollfd* fd_list,int size)
{
    //遍历整个数组，输出文件描述符不为-1的文件描述符
    
    printf("care fd:");
    int index = 0;
    for(;index < size;index++)
    {
        if(fd_list[index].fd != -1)
        {
            printf(" %d ",fd_list[index].fd);
        }
    }
    printf("\n");
    return;
}
int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        printf("Usage:%s [port]\n",argv[0]);
        return 1;
    }


    int port = atoi(argv[1]);

    //得到监听套接字
    int listen_fd = listen_service(port);
    printf("bind and listen success\n");

    //动态申请空间来存放所关心的文件描述符以及其上的事件
    pollfd* fd_list = (pollfd*)malloc(MAXSIZE * sizeof(pollfd));
    
    //初始环数组
    InitArr(fd_list,MAXSIZE);

    //将监听套接字添加进数组中
    AddFd(fd_list,MAXSIZE,listen_fd,POLLIN);

    //利用poll开始等待
    while(1)
    {
        //输出所关心的文件描述符集
        CareFd(fd_list,MAXSIZE);
        int ret = poll(fd_list,MAXSIZE,1000);
        
        //所关心的文件描述符上至少有一个文件描述符有事间就绪
        if(ret > 0)
        {
            //如果是监听套接字就绪，就开始accept接受新链接
            //如果是普通文件描述符就绪，就开始调用read读取数据
            Fd_Server(fd_list,MAXSIZE);
        }
        else if(ret == 0)
        {
            printf("timeout\n");
        }
        else
        {
            printf("poll error\n");
        }
    }


    return 0;
}


