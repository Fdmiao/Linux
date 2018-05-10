#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main()
{
	FILE* fp = fopen("myfile","r");//以只读的方式打开文件myfile，如果文件不存在则打开失败
	if(fp == NULL)//文件打开失败
	{
		perror("fopen error");
		exit(1);
	}

	char buf[1024];
	const char* msg = "hello world\n";
	while(1)
	{
		size_t s = fread(buf,1,strlen(msg),fp);//从fp中读取msg长度的内容写入buf中
		if(s > 0)//如果读取的内容长度大于0
		{
			buf[s] = 0;//在读取的内容后面加\0，以字符串的形式输出
			printf("%s",buf);
			sleep(1);
		}
		else if(feof(fp))//如果读到了文件结尾
		{
			break;//则停止读取
		}
	}
	fclose(fp);//关闭文件
	return 0;
}
