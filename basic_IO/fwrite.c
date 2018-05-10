#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main()
{
	FILE* fp = fopen("myfile","w");//以只写的方式打开myfile文件，如果该文件不存在，就创建它
	if(fp == NULL)//文件打开失败
	{
		perror("fopen error");
		exit(1);//进程退出
	}
	
	const char* msg = "hello world\n";
	int count = 4;
	while(count--)
	{
		fwrite(msg,1,strlen(msg),fp);//往文件中重复写入4条字符串msg
	}

	fclose(fp);//关闭文件
	return 0;
}
