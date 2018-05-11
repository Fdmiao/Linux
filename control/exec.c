#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
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
		int st;
		printf("father is waiting\n");
		int re = wait(&st);//父进程等待
		if(re == -1)//等待失败
		{
			printf("father:wait failed\n");
			exit(1);
		}
		printf("father quit\n");//等待成功
	}
	else//子进程
	{
		char *const arg[]={"ls","-a","--color=auto",NULL,"-l"};
		char *const env[]={"PATH=/bin",NULL};

//		int ret = execl("/bin/ls","ls","-a","-l","--color=auto",NULL);
//		int ret = execlp("ls","ls","-a","-l","--color=auto",NULL);
//		int ret = execle("/bin/ls","ls","-a","-l","--color=auto",NULL,env);
		
		int ret = execv("/bin/l",arg);
//		int ret = execvp("ls",arg);
//		int ret = execve("/bin/ls",arg,env);
		if(ret == -1)//exec函数调用失败
		{
			printf("execl error\n");
		}
		printf("child quit\n");//如果调用失败，就会输出该语句，否则不输出
		exit(1);
	}
	return 0;
//	pid_t pid = fork();
//	if(pid == -1)
//	{
//		printf("%s fork error\n",__FUNCTION__);
//		exit(1);
//	}
//	else if(pid > 0)//父进程
//	{
//		printf("father:pid = %d\n",getpid());
//		int st;
//		printf("father is waiting\n");
//		int re = wait(&st);
//		if(re == -1)
//		{
//			printf("father:wait failed\n");
//		}
//		else
//		{
//			printf("father:wait success!\n");
//			if(WIFEXITED(st))
//			{
//				printf("child exit normal,the exit code of child is %d\n",WEXITSTATUS(st));
//			}
//			else
//			{
//				printf("child exit inormal,the sig code of child is %d\n",st&0x7f);
//			}
//		}
//		printf("father quit\n");
//	}
//	else//子进程
//	{
//		printf("child:pid = %d\n",getpid());
//		char *const arg[]={"ls","-a","--color=auto",NULL,"-l"};
//		char *const env[]={"PATH=/bin",NULL};
//
////		int ret = execl("/bin/ls","ls","-a","-l","--color=auto",NULL);
////		int ret = execlp("ls","ls","-a","-l","--color=auto",NULL);
////		int ret = execle("/bin/ls","ls","-a","-l","--color=auto",NULL,env);
//		
//		int ret = execv("/bin/ls",arg);
////		int ret = execvp("ls",arg);
////		int ret = execve("/bin/ls",arg,env);
//		if(ret == -1)
//		{
//			printf("execl error\n");
//		}
//		printf("child quit\n");
//		exit(1);
//	}
//	return 0;
}
