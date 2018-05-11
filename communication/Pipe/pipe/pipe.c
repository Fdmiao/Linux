#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

//从键盘上读取数据写入管道，从管道读取数据写入屏幕
int main()
{
	int fd[2];
	char buf[100];
	if(pipe(fd) == -1)//通过系统调用接口pipe创建一个管道
	{
		perror("pipe error");
		exit(1);
	}
	
	//模拟一直从管道中读内容的情况	
//	write(fd[1],"hello world\n",strlen("hello world\n"));
//	printf("hello world\n");
//	write(fd[1],"hello world1\n",strlen("hello world1\n"));
//	printf("hello world1\n");
//	while(1)
//	{
//		int len = strlen("hello world1\n");
//		if(read(fd[0],buf,len) == len)
//		  printf("%s\n",buf);
////		read(fd[0],buf,len);
////		printf("%s\n",buf);
//	}

	////模拟一直写的情况
	//while(1)
	//{
	//	write(fd[1],"hello world\n",strlen("hello world"));
	//	printf("hello world\n");
	//}
    //从标准输入即键盘上获取数据保存在buf中
	while(fgets(buf,sizeof(buf),stdin))
	{
		if(write(fd[1],buf,strlen(buf)) == -1)//将获取的数据通过写端写入管道文件中
		{
			perror("write error");
			break;
		}
		memset(buf,0x00,sizeof(buf));//清空buf
		if(read(fd[0],buf,sizeof(buf)) == -1)//通过读端从管道文件中读取数据保存在buf中
		{
			perror("read error");
			break;
		}
		if(write(1,buf,strlen(buf)) == -1)//将从管道中读取的数据写到标准输出即显示器上
		{
			perror("write error");
			break;
		}
    }
//		//模拟没有内容可以读的情况
//		if(read(fd[0],buf,sizeof(buf)) == -1)
//		{
//			perror("read error");
//			break;
//		}
//		if(write(1,buf,strlen(buf)) == -1)
//		{
//			perror("write error");
//			break;
//		}
//
	return 0;
}
