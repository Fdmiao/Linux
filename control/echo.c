#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
int main()
{
//	while(1)
//	{
//		sleep(1);
//	}
	FILE* fp = fopen("js","r");
	if(fp == NULL)//文件打开失败
	{
		printf("%d,%s\n",errno,strerror(errno));//打印错误码和错误描述
	}
//	int i = 0;
//	for(i = 0;i < 200;i++)
//	{
//		printf("%d:%s\n",i,strerror(i));
//	}
	return 0;
}
