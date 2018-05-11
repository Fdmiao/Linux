#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>


//异常退出宏函数
#define ERR_EXIT(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)
	
int main()
{
	umask(0);
	if(mkfifo("mypipe",0644) < 0)//管道创建失败
	{
		ERR_EXIT("mkfifo error");
	}

	int rfd = open("mypipe",O_RDONLY);//以只读方式打开管道文件
	if(rfd < 0)//打开失败
	{
		ERR_EXIT("open error");
	}

	char buf[1024];

	while(1)
	{
		printf("please wait!\n");
		size_t s = read(rfd,buf,sizeof(buf) - 1);//从管道中读取内容
		if(s > 0)
		{
			buf[s] = 0;
			printf("client say#");
			printf("%s\n",buf);		
		}
		else if(s  == 0)
		{
			printf("client quit,me too\n");
		//	exit(EXIT_SUCCESS);
			break;
		}
		else
		{
			ERR_EXIT("read error");
		}
	}
	unlink("mypipe");

	close(rfd);
	return 0;
}

//int main()
//{
//	umask(0);
//	if(mkfifo("mypipe",0644) < 0)//管道创建失败
//	{
//		ERR_EXIT("mkfifo error");
//	}
//
//	int rfd = open("mypipe",O_RDONLY);//以只读方式打开管道文件
//	if(rfd < 0)//打开失败
//	{
//		ERR_EXIT("open error");
//	}
//
//	char buf[1024];
//
//    printf("please wait!\n");
//    size_t s = read(rfd,buf,sizeof(buf) - 1);//从管道中读取内容
//    if(s > 0)
//    {
//        buf[s] = 0;
//        printf("client say#");
//        printf("%s\n",buf);		
//    }
//    else
//    {
//        ERR_EXIT("read error");
//    }
//	unlink("mypipe");
//    close(rfd);//读取内容后关闭读端
//
//	return 0;
//}
