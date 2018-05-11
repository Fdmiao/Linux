
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

//异常退出宏函数
#define ERR_EXIT(m) \
	do \
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)



//读取文件，写入管道
int main()
{
	umask(0);
	if(mkfifo("tp",0644) == -1)//命名管道创建失败
	{
		ERR_EXIT("mkfifo error");
	}

	//创建成功
	int infd = open("abc",O_RDONLY);//以只读方式打开普通文件
	if(infd < 0)//文件打开失败
	{
		ERR_EXIT("infd open error");
	}

	int outfd = open("tp",O_WRONLY);//以只写的方式打开管道文件
	if(outfd < 0)
	{
		ERR_EXIT("outfd open error");
	}

	char buf[1024];
	int s;
	//读取普通文件的内容写入管道文件中
	while((s = read(infd,buf,sizeof(buf))) > 0)//当读取的内容不为0时
	{
		write(outfd,buf,s);//将读取的内容写入管道文件中
	}

	close(infd);
	close(outfd);
	return 0;
}
