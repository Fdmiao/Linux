#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
	int ret=fork();
	if(ret<0)
	{
		printf("fork error!\n");
		return 1;
	}
	else if(ret>0)//父进程
	{	
		printf("farther:pid=%d,ppid=%d,ret=%d\n",getpid(),getppid(),ret);
		sleep(30);
	}
    else//子进程
	{
		printf("child:pid=%d,ppid=%d,ret=%d\n",getpid(),getppid(),ret);
		sleep(5);
		exit(1);
	}
	return 0;
}
