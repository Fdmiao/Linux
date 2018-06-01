#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXSIZE 1024

int listen_server(int port)
{
    //打开网卡文件，获得文件描述符
    int sock = socket(AF_INET,SOCK_STREAM,0);
    //文件描述符获取失败
    if(sock < 0)
    {
        perror("socket error");
        exit(2);
    }

    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = (htonl)INADDR_ANY;
    local.sin_port = htons(port);
    //sock_len_t = sizeof(local);
    
    //对文件描述符绑定固定的IP地址和端口号
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        perror("bind error");
        exit(3);
    }

    //获得监听套接字
    if(listen(sock,5) < 0)
    {
        perror("listen error");
        exit(4);
    }
    //返回获取的监听套接字
    return sock;
}

//初始化数组
void InitArr(int arr[],int size)
{
    //因为文件描述符均是大于等于0的数字
    //所以将数组全部初始化为-1
    int index = 0;
    for(;index < size;index++)
    {
        arr[index] = -1;
    }
    return;
}

//将关心的文件描述符集添加保存到数组中
void AddArr(int arr[],int size,int fd)
{
    //遍历整个数组，遇到-1，就将该文件描述符添加在该处
    int index = 0;
    for(;index < size;index++)
    {
        if(arr[index] == -1)
        {
            arr[index] = fd;
            return;
        }
    }
    //如果循环结束还没有找到-1.此时说明数组全部被占满
    //也就是说文件描述符集所能表示的文件描述符以达到上限
    printf("fd_set full\n");
    return;
}
//根据数组向文件描述符集中添加文件描述符
//同时得到文件描述集中的最大文件描述符
int AddFd(int arr[],int size,fd_set* readfds)
{
    printf("care fd:");
    //遍历数组，遇到一个不是-1的数组元素，将该元素表示的文件描述符添加进文件描述符集中
    int index = 0;
    int max = -1;
    for(;index < size;index++)
    {
        if(arr[index] != -1)
        {
            printf(" %d ",arr[index]);
            FD_SET(arr[index],readfds);
            if(arr[index] > max)
            {
                max = arr[index];
            }
        }
    }
    printf("\n");
    return max;
}
//此时提供接受连接服务
void Accept_Server(int arr[],int size,int listen_fd)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int connfd = accept(listen_fd,(struct sockaddr*)&client,&len);
    if(connfd < 0)
    {
        perror("accept error");
        //continue;
    }
    printf("get a new connect,[%s:%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    //获得新的文件描述符之后，将该文件描述符添加进数组中，以供下一次关心该文件描述符
    AddArr(arr,size,connfd);
}
//将文件描述符从数组中删除
void DelFdInArr(int arr[],int size,int fd)
{
    //遍历整个数组，找到数组元素为要删除文件描述符的下标
    //然后将该下标处的元素置为-1
    int index = 0;
    for(;index < size;index++)
    {
        if(arr[index] == fd)
        {
            arr[index] = -1;
            return;
        }
    }
    return;
}
//此时提供读取数据的服务
void Read_Server(int arr[],int size,int fd)
{
    char buf[10240];

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
        //将文件描述符从数组中删除
        DelFdInArr(arr,size,fd);
    }
    else
    {
        printf("read error");
        close(fd);
        DelFdInArr(arr,size,fd);
    }
    return;
}
//在该函数中根据数组中记录的所关心的文件描述符集先判断哪个文件描述符就绪
//如果是监听文件描述符，则调用accept接受新连接
//如果是普通文件描述符，则调用read读取数据
void server_fd(int arr[],int size,fd_set* readfds)
{
    //首先遍历数组，找到不是-1的文件描述符
    //然后在文件描述符集中判断该文件描述符是否存在
    //因为监听套接字被最先设定，所以一定存放在数组下标为0的位置
    //下标为0的数组元素出现在文件描述符集中，一定是监听套接字
    int index = 0;
    for(;index < size;index++)
    {
        //该文件描述符为所关心的文件描述符
        if(arr[index] != -1 && FD_ISSET(arr[index],readfds))
        {
            //该文件描述符是监听套接字
            if(index == 0)
            {
                //如果监听套接字上读就绪
                //此时提供接受连接服务
                Accept_Server(arr,size,arr[index]);
            }
            else
            {
                //此时关心的是普通文件描述符
                //此时提供读取数据的服务
                Read_Server(arr,size,arr[index]);
            }
        }
    }
}
int main(int argc,char* argv[])
{
    //用法说明
    if(argc != 2)
    {
        printf("Usage:%s [port]\n",argv[0]);
        return 1;
    }
    //创建文件描述符，将该文件描述符绑定，监听，得到监听套接字
    //根据参数传入的端口号进行绑定,端口号传入的时候是以字符串的形式传入的
    //所以现将它转化为整型
    int port = atoi(argv[1]);
    int listen_fd = listen_server(port);
    printf("bind and listen success\n");

    //定义只读文件描述符集
    fd_set readfds;
    
    //定义数组将关心的文件描述符集保存在数组中
    int arr[MAXSIZE];
    //初始化数组
    InitArr(arr,MAXSIZE);

    //将关心的文件描述符集添加保存到数组中
    //首先将监听套接字添加进去
    AddArr(arr,MAXSIZE,listen_fd);

    while(1)
    {
        //清空文件描述符集
        FD_ZERO(&readfds);

        //根据数组向文件描述符集中添加文件描述符
        //同时得到文件描述集中的最大文件描述符
        int max = AddFd(arr,MAXSIZE,&readfds);

        //开始调用select等待所关心的文件描述符集
        int ret = select(max + 1,&readfds,NULL,NULL,NULL);
        
        //此时关心的文件描述符集中至少有一个文件描述符读就绪
        if(ret > 0)
        {
            //在该函数中根据数组中记录的所关心的文件描述符集先判断哪个文件描述符就绪
            //如果是监听文件描述符，则调用accept接受新连接
            //如果是普通文件描述符，则调用read读取数据
            server_fd(arr,MAXSIZE,&readfds);
        }
        else if(ret == 0)
        {
            //此时表示超时返回，此时没有文件描述符发生读就绪
            printf("timeout\n");
        }
        else
        {
            //此时，select调用失败
            perror("select error");
            continue;
        }
    }
    return 0;
}
