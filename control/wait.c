#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
int main()
{
	pid_t ret = fork();//创建子进程
	if(ret == -1)
	{
		perror("fork");
		exit(1);
	}
	else if(ret > 0)//父进程
	{
		printf("father:pid = %d,ret = %d\n",getpid(),ret);
		int st;
		printf("father is waiting\n");
		int ret1 = wait(&st);//父进程等待子进程退出
		if(ret1 == -1)//如果wait调用失败
		{
			printf("wait failed,wait return %d\n",ret1);
		}
		else//wait调用成功
		{
			printf("wait success,wait return %d\n",ret1);
	    	if((st & 0x7f) == 0)//，如果st的低7位等于0，即子进程正常退出，可替换为if(WIFEXITED(st))
			{
				printf("child exit normal,sig code:%d,exit code:%d\n",st & 0x7f,(st >> 8) & 0xff);//打印退出码
			}
			else//如果低7位不为0，即子进程异常退出
			{
				printf("child exit unnormal,sig code:%d\n",st & 0x7f);//打印信号值
			}
		}
		printf("father quit\n");//子进程退出之后，父进程才退出
	}
	else//子进程在运行
	{
		printf("child is running\n");
		printf("child:pid = %d,ret = %d\n",getpid(),getppid(),ret);
		int count = 5;
		while(count--)
		{
			printf("count = %d\n",count);
			sleep(2);
		}
		printf("child quit\n");
		exit(1);//令子进程的退出码为1
	}


//	pid_t ret = fork();
//	if(ret == -1)
//	{
//		perror("fork");
//		exit(1);
//	}
//	else if(ret > 0)//父进程
//	{
//		printf("father:pid = %d,ppid = %d,ret = %d\n",getpid(),getppid(),ret);
//		pid_t ret1 = wait(NULL);
//		if(ret1 == -1)
//		{
//			printf("wait error,%d\n",ret1);
//		}
//		else
//		{
//			printf("wait success:%d\n",ret1);
//		}
//		printf("father quit\n");
//	}
//	else//子进程
//	{
//		printf("child:pid = %d,ppid = %d,ret = %d\n",getpid(),getppid(),ret);
//		int count = 5;
//		while(count--)
//		{
//			printf("count = %d\n",count);
//			sleep(1);
//		}
//		printf("child quit\n");
//		exit(1);
//	}




//	pid_t pid = fork();
//	if(pid == -1)
//	{
//		perror("fork");
//		exit(1);
//	}
//	else if(pid == 0)
//	{
//		sleep(20);
//		exit(1);
//	}
//	else
//	{
//		int st;
//		int ret = wait(&st);
//		if(ret > 0 )
//		{
//			printf("child :exit code:%d sig code:%d\n",(st >> 8)& 0xff,st&0x7f);
//		}
//
//	}
	return 0;
}
