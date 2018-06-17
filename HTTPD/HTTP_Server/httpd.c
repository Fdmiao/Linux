#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include<sys/wait.h>
#include<ctype.h>

#define MAX 1024
#define HOME_PAGE "index.html"

static void Usage(const char* proc)
{
	printf("Usage:%s [port]\n",proc);
}

static int startup(int port)
{
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("socket");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		perror("bind");
		exit(3);
	}

	if(listen(sock,5) < 0)
	{
		perror("listen");
		exit(4);
	}
	return sock;
}

//由于不知到对方客户端的类型，所以无法判断其行分割符是\r,\n,\r\n
//将行分割符统一转化为\n
static int get_line(int sock,char line[],int size)
{
	int c = 'a';
	int i = 0;
	ssize_t s = 0;
	while(i < size-1 && c != '\n') //i < size-1 因为字符串结尾有\0
	{
		s = recv(sock, &c, 1, 0);
		if(s > 0)
		{
			if(c == '\r')
			{
				recv(sock, &c, 1, MSG_PEEK);
				// \r --> \n
				if(c != '\n') 
					c = '\n';
				// \r\n -->\n
				else
					recv(sock, &c, 1, 0);
			}
			line[i++] = c;
		}
		else
			break;
	}
	line[i] = '\0';
	return i;
}

void head_clear(int sock)
{
	char line[MAX];
	do
	{
		get_line(sock,line,sizeof(line));
	}while(strcmp(line,"\n") != 0);
}

void echo_www(int sock,char* path,int size,int* err)
{
	//HTTP服务器是基于请求响应的，
	//所以必须将请求全部读完，才开始响应
	head_clear(sock);
	
	int fd = open(path,O_RDONLY);
	if(fd < 0)
	{
		*err = 404;
		return;
	}

	//line作为响应的缓冲区
	//将版本号，响应报头，空行全部发送至sock
	char line[MAX];
	sprintf(line,"HTTP/1.0 200 OK\r\n");
	send(sock,line,strlen(line),0);
	sprintf(line,"Content-Type:text/html\r\n");
	send(sock,line,strlen(line),0);

	sprintf(line,"\r\n");
	send(sock,line,strlen(line),0);
	//调用sendfile将响应的资源发送给sock
	sendfile(sock,fd,NULL,size);//sendfile直接从内核发送，更高效
	close(fd);
}

int exe_cgi(int sock,char path[],char method[],char* query_string)
{
	int content_length = -1;
	char line[MAX];
	char method_env[MAX/32];
	char query_string_env[MAX];
	char content_length_env[MAX/16];
	
	if(strcasecmp(method,"GET") == 0)
	{
		head_clear(sock);//把请求读完
	}
	else//POST
	{
		do
		{
			get_line(sock,line,sizeof(line));
			if(strncmp(line,"Content-Length: ",16) == 0)
			{
				content_length = atoi(line+16);
			}
		}while(strcmp(line,"\n") != 0);
	
		//POST方法，没有读到任何内容
		if(content_length == -1)
			return 404;
	}

	//HTTP响应的响应行 报头 空行
	sprintf(line,"HTTP/1.0 200 OK\r\n");
	send(sock,line,strlen(line),0);
	sprintf(line,"Content-Type:text/html\r\n");
	send(sock,line,strlen(line),0);
	sprintf(line,"\r\n");
	send(sock,line,strlen(line),0);

	//建立两个管道，使父子进程进行通信
	//子进程input从管道中读数据
	//父进程output往管道中写数据
	int input[2];
	int output[2];
	pipe(input);
	pipe(output);

	pid_t id = fork();
	if(id < 0)
		return 404;
	else if(id == 0)//child read->input[0]  write->output[1]
	{
		close(input[1]);
		close(output[0]);
		//将input[0]重定向到标准输入,output[1]重定向到标准输出
		//这样，子进程的输入输出就是直接往管道里读写数据
		dup2(input[0],0);
		dup2(output[1],1);

		//method,query_string,content_length这三个变量父子进程都需要看到
		//我们可以将它们都放进管道中，但管道的内容就不只是请求的资源了,面向字节流，不容易区分
		//环境变量具有全局性，进程替换不会替换环境变量，可以将他们都设置为环境变量
		sprintf(method_env,"METHOD=%s",method);
		putenv(method_env);

		if(strcasecmp(method,"GET") == 0)
		{
			sprintf(query_string_env,"QUERY_STRING=%s",query_string);
			putenv(query_string_env);
		}
		
		if(strcasecmp(method,"POST") == 0)
		{
			sprintf(content_length_env,"CONTENT_LENGTH=%d",content_length);
			putenv(content_length_env);
		}

		//程序替换，替换为path中请求的资源,返回运行结果
		execl(path,path,NULL);
		exit(1);
	}
	else//father read->output[0] write->input[1]
	{
		close(input[0]);
		close(output[1]);

		char c;
		//对于POST方法，只知道content_length，并不知道请求的资源到底是什么
		//父进程读取HTTP请求正文，将请求资源写入管道
		if(strcasecmp(method,"POST") == 0)
		{
			int i = 0;
			while(i < content_length)
			{
				read(sock,&c,1);
				write(input[1],&c,1);
				++i;
			}
		}

		//else
		//{

		//}

		//不管是GET POST方法，请求的资源现在都在管道里面
		//只需要从管道里读取运行结果，转发给客户
		while(read(output[0],&c,1) > 0)
		{
			send(sock,&c,1,0);
		}

		waitpid(id,NULL,0);
		close(input[1]);
		close(output[0]);
	}
	return 200;
}

void echo_err(int code)
{
	switch(code)
	{
		case 404:
			break;
		case 501:
			break;
		default:
			break;
	}
}
void* handler_request(void* arg)
{
	int sock = (int) arg;
	char line[MAX];
	int errcode = 200;
	char method[MAX/32];
	char url[MAX];
	char path[MAX];
	char* query_string = NULL;
	int cgi = 0;
	//do
	//{
	//	get_line(sock,line,sizeof(line));
	//	printf("%s",line);
	//}while(strcmp(line,"\n") != 0);
	
	if(get_line(sock,line,sizeof(line)) < 0)
	{
		errcode = 404;
		goto end;
	}

	//get method
	int i = 0,j = 0;
	while(i < sizeof(method)-1 && j < sizeof(line) && !isspace(line[j]))
	{
		method[i] = line[j];
		i++;
		j++;
	}
	method[i] = '\0';

	//printf("method:%s\n",method);
	//只处理GET方法和POST方法
	//strcasecmp忽略大小写比较字符串，因为请求方法没有规定一定是全大写
	//post,Post,POST,poST
	if(strcasecmp(method,"GET") == 0)
	{
	}
	else if(strcasecmp(method,"POST") == 0)
	{
		cgi = 1;
	}
	else
	{
		errcode = 404;
		goto end;
	}

	//把method和url之间的空格走完
	while(j < sizeof(line) && isspace(line[j]))
	{
		j++;
	}

	//get url
	i = 0;
	while(i < sizeof(url)-1 && j < sizeof(line) && !isspace(line[j]))
	{
		url[i] = line[j];
		i++,j++;
	}
	url[i] = '\0';

	//GET方法，将资源url和参数query_string分离开
	if(strcasecmp(method,"GET") == 0)
	{
		query_string = url;
		while(*query_string)
		{
			if(*query_string == '?')
			{
				*query_string = '\0';
				++query_string;
				cgi = 1;
				break;
			}
			else
				++query_string;
		}
	}

	//获得请求资源的路径
	sprintf(path,"wwwroot%s",url);
	//路径path有两种情况
	//1 普通路径：wwwroot/a/b/c 
	//2 根目录:wwwroot/
	if(path[strlen(path)-1] == '/')
	{
		strcat(path,HOME_PAGE);
	}
	
	printf("method:%s path:%s\n",method,path);

	//stat函数判断请求资源（文件）是否存在,还可以拿到对应文件的属性信息
	struct stat st;
	//文件不存在
	if(stat(path,&st) < 0)
	{
		errcode = 404;
		goto end;
	}
	//文件存在
	else
	{
		//判断文件是否为目录文件
		//是，显示首页（或者为wwwroot的每一个目录建立首页）
		if(S_ISDIR(st.st_mode))
		{
			strcat(path,HOME_PAGE);
		}
		else
		{
			//判断文件是否具有可执行权限，是，以cgi方式运行
			if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
			{
				cgi = 1;
			}
		}

		//以cgi模式运行
		if(cgi)
		{
			errcode = exe_cgi(sock,path,method,query_string);
		}
		//以普通直接显示页面方式,GET方法，指定资源在path中，没有参数
		else
		{
			echo_www(sock,path,st.st_size,&errcode);
		}
	}
	

end:
	if(errcode != 200)
	{
		echo_err(errcode);
	}
	close(sock);
}

int main(int argc,char* argv[])
{
	if(argc != 2)
	{
		Usage(argv[0]);
		return 1;
	}

	int listen_sock = startup(atoi(argv[1]));
	signal(SIGPIPE,SIG_IGN);//忽略SIG_PIPE信号
	while(1)
	{
		struct sockaddr_in client;
		socklen_t len = sizeof(client);
		int new_sock = accept(listen_sock,(struct sockaddr*)&client,&len);
		if(new_sock < 0)
		{
			perror("accept");
			continue;
		}

		pthread_t id;
		pthread_create(&id,NULL,handler_request,(void*)new_sock);
		pthread_detach(id);
	}
	return 0;
}
