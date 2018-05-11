#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
int main()
{
	//FILE* fp = popen("cat","w");
	//if(fp == NULL)
	//{
	//	perror("popen");
	//	exit(1);
	//}
	//fwrite("hello world\n",1,strlen("hello world\n"),fp);
	//pclose(fp);
	FILE* fp = popen("ls -a","r");
	if(fp == NULL)
	{
		perror("popen");
		exit(1);
	}
	char buf[4096];
	fread(buf,1,sizeof(buf),fp);
	printf("%s\n",buf);
	pclose(fp);
	return 0;
}
