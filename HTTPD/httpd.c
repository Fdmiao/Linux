#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<sys/sendfile.h>
#include<sys/wait.h>
#include<signal.h>

#define MAX 1024
#define HOME_PAGE "index.html"
#define PAGE_404 "404.html"

//获得监听套接字
int service(int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("socket error");
        exit(2);
    }

    //将套接字设置为可以绑定不同IP地址的相同端口号
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(port);
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        perror("bind error");
        exit(3);
    }

    if(listen(sock,5) < 0)
    {
        perror("listen error");
        exit(4);
    }

    return sock;

}


//处理请求报头
//获取请求报头的每一行
//一行的结束有三种情况：\n,\r,\r\n
static int get_line(int sock,char* line,int size)
{
    char c ='a';//c的初始化任意，只要使c不为\n，能够进入循环即可
    int i = 0;
    //在while中只处理\r,\r\n的情形
    //读到\n时，说明读到一行的行尾了
    while(i < size - 1 && c != '\n')
    {
        //一次读取一个字符
        int s = recv(sock,&c,1,0);
        if(s > 0)
        {
            if(c == '\r')
            {
                //窥探性的读，只看到该字符的内容，但不将该字符从缓冲区中取走
                recv(sock,&c,1,MSG_PEEK);
                //  \r
                //如果\r之后的字符不是\n，说明行尾的标记为\r，此时只需将\r改为\n，即可表示到达行尾了
                if(c != '\n')
                {
                    c = '\n';
                }
                //  \r\n
                //如果\r之后的字符是\n,说明行尾的标记为\r\n，此时只需将\n读取出来，即可得到\n从而到达行尾
                else
                {
                    recv(sock,&c,1,0);
                }
            }
            //读取一个字符，向line中写入一个字符
            line[i++] = c;
        }
        else
        {
            break;
        }
    }
    //读完一行之后，在\n之后加\0表示字符串的结尾
    line[i] = '\0';
    return i + 1;//返回一行中的字符数
}

//清空请求报头
void head_clear(int sock)
{
    char line[MAX];
    do
    {
        get_line(sock,line,sizeof(line));
    }while(strcmp(line,"\n") != 0);
    return;
}
void echo_www(int sock,char* path,int size,int* errcode)
{
    //在向浏览器返回响应报文时，首先要将请求报文读取完
    head_clear(sock);

    //以只读方式打开资源文件
    int fd = open(path,O_RDONLY);
    if(fd < 0)
    {
        *errcode = 404;
        return;
    }
    //读完之后，再将响应首行，响应报头，空行，响应正文发送过去
    char line[MAX];//存放响应首行，响应报头
    
    //发送响应首部
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);

    //发送响应报头
    sprintf(line,"Content-Type:text/html\r\n");
    send(sock,line,strlen(line),0);

    //发送空行
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);

    //发送响应正文
    //直接从内核发送，效率高
    sendfile(sock,fd,NULL,size);

    close(fd);

}
//运行cgi模式
int exe_cgi(int sock,char* path,char* method,char* query_string)
{
    int content_length = -1;
    char line[MAX];
    char method_env[MAX];
    char query_string_env[MAX];
    char content_length_env[MAX];
    if(strcasecmp(method,"GET") == 0)
    {
        head_clear(sock);
    }
    //如果是POST方法，现在只知道资源路径，方法，还不知道参数
    //  因为参数在请求正文中存放，在读取时，为防止粘包问题，首先要知道请求正文的起始位置和长度
    //  所以在清空请求报头的过程中顺带将参数的长度读取出来
    else if(strcasecmp(method,"POST") == 0)
    {
        //在清空请求报头的同时，获取参数长度
        do
        {
            //读取一行
            get_line(sock,line,sizeof(line));
            //如果该行中有Content-Length: 字段，则在该字段之后的内容即为参数的长度即请求正文长度
            if(strncmp(line,"Content-Length: ",16) == 0)
            {
                content_length = atoi(line + 16);
            }
        }while(strcmp(line,"\n") != 0);
        //如果没有读到任何内容,此时对应的是请求报头中没有Content-Length的字段
        if(content_length == -1)
        {
            return 404;
        }
    }
    else//如果是其他方法直接返回错误码
    {
        return 404;
    }

    //请求报文读取完之后，要发送响应报文(有问题：POST方法中的请求正文还没有读取完)
    //发送响应首部
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);
    //发送响应报头
    sprintf(line,"Content-Type:text/html\r\n");
    send(sock,line,strlen(line),0);
    //发送空行
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);

    //响应报头发送完之后，要发送响应正文，所以首先要处理请求，从而得到响应正文
    //因为请求报文是带参数的可执行程序，所以可以创建子进程进行程序替换执行这个可执行程序
    //子进程在执行可执行程序时，要得到该程序的资源即路径，方法，参数

    //如果是GET方法，这三者已经得到，现在要考虑如何将它们传递给子进程
    //子进程进行程序替换时，可以直接根据路径去执行，所以，只需要将方法和参数传递给子进程
    //因为环境变量具有全局性，父子进程可以共享环境变量，而不论子进程的程序有没有替换
    //所以，将方法和参数设置为环变量传递给子进程
    
    //如果是POST方法，同理，将方法和参数的长度设置为环境变量传递给子进程

    //注意，父子进程均可以导出环境变量

    //无论是GET还是POST方法，均将方法设置为环境变量
    sprintf(method_env,"METHOD=%s",method);
    putenv(method_env);

    //如果是GET方法，将参数导出为环境变量
    if(strcasecmp(method,"GET") == 0)
    {
        sprintf(query_string_env,"QUERY_STRING=%s",query_string);
        putenv(query_string_env);
    }

    //如果是POST方法将参数长度导出为环境变量
    if(strcasecmp(method,"POST") == 0)
    {
        sprintf(content_length_env,"CONTENT_LENGTH=%d",content_length);
        putenv(content_length_env);
    }

    //因为对于POST方法，子进程还不知道参数的具体内容，所以还需要父进程来提供
    //父进程要从sock文件中将正文读取出来，可以通过管道传输给子进程
    //子进程执行程序后的结果也通过管道来发送给父进程
    //所以，父子进程均要从管道中进行读写，因此需要建立双向管道

    int input[2];
    int output[2];
    pipe(input);
    pipe(output);

    //这里站在子进程的角度来进行读写
    //子进程要从管道中读，可以从input[0]中读，要写入管道，所以，要写入output[1]
    //相应的，父进程要从output[0]中读，写入input[1]
    //因此，父子进程要关闭不用的管道文件描述符
    
    //将管道重定向到输入输出文件中（有问题：为什么）

    //创建子进程进行程序替换
    int pid = fork();
    if(pid < 0)//子进程创建失败
    {
        return 404;
    }
    else if(pid == 0)//子进程
    {
        //子进程关闭不用的读端和写端
        close(input[1]);//关闭父进程的写端
        close(output[0]);//关闭父进程的读端

        //将管道重定向到输入输出文件中,dup2(oldfd,newfd):新的是旧的一份拷贝
        dup2(input[0],0);
        dup2(output[1],1);
        execl(path,path,NULL);
        exit(1);
    }
    else//父进程
    {
        //关闭子进程的读写端
        close(input[0]);
        close(output[1]);

        //只有POST方法，父进程才要从sock中读取请求正文即参数
        //然后再将参数写入管道中
        char c;
        if(strcasecmp(method,"POST") == 0)
        {
            int i = 0;
            for(;i < content_length;i++)
            {
                read(sock,&c,1);
                write(input[1],&c,1);
            }
        }

        //无论是POST方法，还是GET方法，父进程最后都要从管道中读取结果，再将结果写入sock中
        while(1)
        {
            int s = read(output[0],&c,1);
            if(s < 0)
            {
                perror("read");
                return 404;
            }
            else if(s == 0)
            {
                break;
            }
            else
            {
                send(sock,&c,1,0);
            }
        }

        waitpid(pid,NULL,0);
        close(input[1]);
        close(output[0]);
        close(sock);//有问题：这里要不要关闭sock
    }
    return 200;
}

void show_404(int sock)
{
    head_clear(sock);
    char path[1024];
    sprintf(path,"wwwroot/404.html");
    int fd = open(path,O_RDONLY);
    char line[MAX];//存放响应首行，响应报头

    struct stat st;
    stat(path,&st);

    //发送响应首部
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);

    //发送响应报头
    sprintf(line,"Content-Type:text/html\r\n");
    send(sock,line,strlen(line),0);

    //发送空行
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);

    //发送响应正文
    //直接从内核发送，效率高
    sendfile(sock,fd,NULL,st.st_size);

    close(fd);
}

void echo_err(int sock,int code)
{
	switch(code)
	{
		case 404:
            show_404(sock);
			break;
		case 501:
			break;
		default:
			break;
	}
}

void* handler_request(void* arg)
{
    int sock = (int)arg;
    char line[MAX];//存放请求报头中的一行
    char method[MAX/32];//存放请求行中的方法
    char url[MAX];//存放请求行中的URL
    char path[MAX];
    int errcode = 200;
    int cgi = 0;
    //char query_string[MAX];

    //处理请求首行，获取请求方法，URL
    int s = get_line(sock,line,sizeof(line));
    //请求行获取失败
    if(s < 0)
    {
        errcode = 404;
        goto end;
    }
    int i = 0;//表示methed和url的下标
    int j = 0;//表示line的下标
    
    //获取请求方法
    while(i < sizeof(method) - 1 && j < sizeof(line) && !isspace(line[j]))
    {
        method[i] = line[j];
        i++;
        j++;
    }
    method[i] = '\0';

    //跳过methon和url之间的空格（可能有多个）
    while(j < sizeof(line) && isspace(line[j]))
    {
        j++;
    }

    //获取url
    i = 0;
    while(i < sizeof(url) - 1 && j < sizeof(line) && !isspace(line[j]))
    {
        url[i] = line[j];
        i++;
        j++;
    }
    url[i] = '\0';

    //根据请求方法判定是否使用cgi
    //如果是GET方法，且不带参数，则不使用cgi
    //               如果带参数，则使用cgi
    //如果是POST方法，一定带参数，则使用cgi
    //根据url提取出参数
    
    char* query_string = NULL;
    //如果是GET方法:使用strcasecmp比较时忽略大小写
    if(strcasecmp(method,"GET") == 0)
    {
        query_string = url;
        //循环结束后，如果query_string中只有\0，则表示GET方法不带参数
        //否则为GET方法带参数，此时cgi为1
        while(*query_string != '\0')
        {
            //如果GET方法带参数，则使用cgi
            if(*query_string == '?')
            {
                *query_string = '\0';
                query_string++;
                cgi = 1;
                break;
            }
            else
            {
                query_string++;
            }
        }
    }
    //如果是POST方法,则一定带参数，此时令cgi为1
    else if(strcasecmp(method,"POST") == 0)
    {
        cgi = 1;
    }
    else//如果是其他方法，直接出错处理
    {
        errcode = 404;
        goto end;
    }

    //将资源和根目录拼接在一起
    sprintf(path,"wwwroot%s",url);

    printf("method:%s,path:%s\n",method,path);

    struct stat st;
    int ret = stat(path,&st);
   
    //申请的资源不存在
    if(ret < 0)
    {
        errcode = 404;
        goto end;
    }
    //如果申请的资源存在
    else
    {
        //如果申请的是目录：根目录或普通目录
        if(S_ISDIR(st.st_mode))
        {
            strcat(path,HOME_PAGE);
        }
        ////申请的是根目录
        //if(path[strlen(path) - 1] == '/')
        //{
        //    strcat(path,HOME_PAGE);
        //}
        ////申请的资源是目录,将主页面拼接起来
        //else if(S_ISDIR(st.st_mode))
        //{
        //    strcat(path,HOME_PAGE);
        //}
        //如果是可执行文件,运行cgi模式
        else if(st.st_mode & S_IXOTH || st.st_mode & S_IXGRP || st.st_mode & S_IXUSR)
        {
            cgi = 1;
        }
        //以cgi模式运行
        if(cgi == 1)
        {
            errcode = exe_cgi(sock,path,method,query_string);
        }
        //GET方法，没有参数，直接将资源传送给sock，传递给浏览器即可
        else
        {
            echo_www(sock,path,st.st_size,&errcode);    
        }
    }
    end:
    if(errcode != 200)
    {
        echo_err(sock,errcode);
    }
    //测试获取请求报头
//    do
//    {
//        get_line(sock,line,sizeof(line));
//        printf("%s",line);
//    }while(strcmp(line,"\n") != 0);
    close(sock);
}

int main(int argc,char* argv[])
{
    //用法说明
    if(argc != 2)
    {
        printf("Usage:%s [port]\n",argv[0]);
        return 1;
    }
    //获得监听套接字
    int listen_sock = service(atoi(argv[1]));
    signal(SIGPIPE,SIG_IGN);
    printf("bind and listen success...\n");

    //开始接受新链接
    while(1)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int connfd = accept(listen_sock,(struct sockaddr*)&client,&len);
        if(connfd < 0)
        {
            perror("accept error");
            continue;
        }
        else
        {
            printf("get a new connect:[ip:%s][port:%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
            pthread_t tid;
            pthread_create(&tid,NULL,handler_request,(void*)connfd);
            pthread_detach(tid);
        }
    }


    return 0;
}
