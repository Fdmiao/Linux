#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main()
{
	pid_t ret = vfork();
	if(ret == -1)
	{
		perror("fork");
		exit(1);
	}
	if(ret > 0)
	{
		printf("f\n");
	}
	else
	{
		printf("c\n");
		_exit(1);
	}
//	int g_val = 100;//设置全局变量
//	pid_t ret = vfork();
//	if(ret == -1)
//	{
//		perror("vfork");
//		exit(1);
//	}
//	if(ret > 0)//父进程
//	{
//		g_val = 3;
//		printf("father:pid = %d,g_val = %d,ret= %d\n",getpid(),g_val,ret);
//	//	printf("%p\n",&g_val);
//	}
//	else//子进程
//	{
//		g_val = 50;
//		printf("child:pid %d,g_val = %d,ret = %d\n",getpid(),g_val,ret);
//		printf("%p\n",&g_val);
//		sleep(1);
//		//return 0;
//		_exit(0);
//	}
//	return 0;
}
