#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main()
{
	close(1);
	int fd = open("myfile",O_WRONLY|O_CREAT,0644);
	if(fd < 0)
	{
		perror("open error");
		exit(1);
	}
	printf("fd:%d\n",fd);
	fflush(stdout);
	close(fd);
	return 0;
//	char buf[1024];
//	ssize_t s = read(0,buf,sizeof(buf));//从标准输入即键盘上读取内容到buf中
//	if(s > 0)
//	{
//		write(1,buf,strlen(buf));//将buf的内容写到标准输出即显示器上
//		write(2,buf,strlen(buf));//将buf的内容写到标准错误即显示器上
//
//	}
//	return 0;
}
