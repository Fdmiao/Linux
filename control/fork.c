#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
int main()
{
	pid_t pid = fork();
	if(pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if(pid > 0)//父进程
	{
		printf("father:pid = %d,ppid = %d\n",getpid(),getppid());
		sleep(1);
	}
	else//子进程
	{
		printf("child:pid = %d,ppid = %d\n",getpid(),getppid());
	}
	return 0;
}
