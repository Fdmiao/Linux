#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
int main()
{
	pid_t pid = fork();
	if(pid == -1)
	{
		printf("%s fork error\n",__FUNCTION__);
		exit(1);
	}
	else if(pid > 0)//父进程
	{
		printf("father is running\n");
		printf("father:pid = %d\n",getpid());
		printf("father is waiting\n");
		int ret;
		int st;
		do
		{
			ret = waitpid(-1,&st,WNOHANG);//非阻塞式等待
			if(ret == 0)//子进程没有结束，父进程就做其他事情
			{
				printf("child is running:\n");
				sleep(1);
			}
		}while(ret == 0);
		printf("wait end\n");
		if(WIFEXITED(st))
		{
			printf("child exit normal,the exit code of child is %d\n",WEXITSTATUS(st));
		}
		else
		{
			printf("child exit inormal,the exit sig of child is %d\n",st&0x7f);
		}
		printf("father quit\n");
	}
	else//子进程
	{
		//printf("child is running\n");
		printf("child:pid = %d\n",getpid());
		sleep(5);
//		int count = 8;
//		while(count--)
//		{
//			printf("count = %d\n",count);
//			sleep(1);
//		}
		printf("child quit\n");
		exit(50);
	}
//	pid_t pid = fork();
//	if(pid == -1)
//	{
//		perror("fork error");
//		exit(1);
//	}
//	else if(pid > 0)//父进程
//	{	
//		printf("father is running\n");
//		printf("father:pid = %d\n",getpid());
//		printf("father is waiting\n");
//		int st;
//		int ret = waitpid(-1,&st,0);//阻塞式等待任意进程
//		printf("wait end\n");
//		if(ret == -1)
//		{
//			printf("wait failed\n");
//		}
//		else
//		{
//			printf("wait success!,wait return %d\n",ret);
//			if(WIFEXITED(st))//可替换为:if(!(st&0x7f))
//			{
//				printf("child exit normal! exit code of child is %d\n",WEXITSTATUS(st));
//			}
//			else
//			{
//				printf("childe exit inormal! sig code of child is %d\n"st&0x7f);
//			}
//		}
//	}
//	else//子进程
//	{
//		printf("child is running\n");
//		printf("child:pid = %d\n",getpid());
//		int count = 5;
//		while(count--)
//		{
//			printf("count = %d\n",count);
//			sleep(3);
//		}
//		exit(20);
//	}
	return 0;
}
