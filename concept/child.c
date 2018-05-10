#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
	int ret = fork();
	if(ret == -1)
	{
		perror("fork error\n");
		return 1;
	}
	else if(ret > 0)//父进程
	{
		printf("pid = %d,ppid = %d ret = %d\n",getpid(),getppid(),ret);
	}
	else
	{

		printf("pid = %d,ppid = %d ret = %d\n",getpid(),getppid(),ret);
		sleep(30);
	}
	return 0;
}
