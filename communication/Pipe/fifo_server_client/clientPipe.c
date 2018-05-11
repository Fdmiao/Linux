#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

#define ERR_EXIT(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

int main()
{
	int wfd = open("mypipe",O_WRONLY);//以只写的方式打开管道文件
	if(wfd < 0)//打开失败
	{
		ERR_EXIT("open error");
	}

	char buf[1024];

	while(1)
	{
		printf("please enter#");
		fflush(stdout);
        //scanf("%s",buf);
        //write(wfd,buf,strlen(buf));
		size_t s = read(0,buf,sizeof(buf)-1);//从标准输入中读取内容
		if(s > 0)
		{
			buf[s] = 0;
			write(wfd,buf,strlen(buf));//将读取的内容写入管道文件
		}
		else
		{
			ERR_EXIT("read error");
		}
	}
	close(wfd);

	return 0;
}

//``void handler(int signo)
//``{
//``    printf("catch signo %d\n",signo);
//``    exit(1);
//``}
//``
//``int main()
//``{
//``    signal(SIGPIPE,handler);//自定义SIGPIPE信号的处理动作
//``	int wfd = open("mypipe",O_WRONLY);//以只写的方式打开管道文件
//``	if(wfd < 0)//打开失败
//``	{
//``		ERR_EXIT("open error");
//``	}
//``
//``	char *buf = "hello world\n";
//``
//``	while(1)
//``	{
//``        write(wfd,buf,strlen(buf));//写端一直往管道中写入内容
//``	}
//``	close(wfd);
//``
//``	return 0;
//``}
