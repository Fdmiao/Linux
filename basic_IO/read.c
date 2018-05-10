#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
int main()
{
	int fd = open("myfile1",O_RDONLY);//以只读的方式打开文件
	if(fd < 0)//文件打开失败
	{
		perror("open error");
		exit(1);
	}

	char buf[1024];
	const char* msg = "hello world\n";
	while(1)
	{
		ssize_t s = read(fd,buf,strlen(msg));
		if(s > 0)
		{
			buf[s] = 0;
			printf("%s",buf);
			sleep(1);
		}
		else if(s == 0)//文件到达结尾
		{
			break;
		}
	}
	close(fd);
	return 0;
	////close(0);
	////close(2);
	//close(1);
	//int fd = open("myfile",O_WRONLY);
	//if(fd < 0)
	//{
	//	perror("open error");
	//	return 1;
	//}

	//printf("nakl  %d\n",fd);
	//fork();
    ////fflush(stdout);
	////close(fd);
	//exit(0);
	////int fd = open("myfile",O_RDONLY);
	////if(fd < 0)
	////{
	////	perror("open error");
	////	return 1;
	////}
	////const char* msg = "hello bit\n";
	////char buf[1024];
	////while(1)
	////{
	////	size_t s = read(0,buf,strlen(msg));//从标准输入读取内容到buf中
//	////	size_t s = read(fd,buf,strlen(msg));//从文件fd中的读取内容到buf
	////	if(s > 0)
	////	{
	////		buf[s] = 0;
	////		printf("%s",buf);
	////	}
	////	else
	////	{
	////		break;
	////	}
	////}
	////close(fd);
	////return 0;
}
