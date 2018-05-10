#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
	umask(0);//消除掩码对权限造成的影响
	int fd = open("myfile1",O_WRONLY|O_CREAT,0644);//创建文件myfile1，并以只写的方式打开
	if(fd < 0)//文件打开失败
	{
		perror("open");
		return 1;
	}
	const char* msg = "hello world\n";
	int count = 4;
	while(count--)
	{
		write(fd,msg,strlen(msg));
	}
	close(fd);
	return 0;
}
