#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

char* arg[20] = {};
int count = 0;

//解析字符串
void do_parse(char* buf)
{
	int i = 0;
	int status = 0;//定义状态变量，0表示为空格状态，1表示非空格状态
	for(count = i = 0;buf[i] != '\0';i++)
	{
		if((!isspace(buf[i])) && (status == 0))//当遇到非空格且状态原状态为0，及表示有空格进入到非空个时
		{
			arg[count++] = buf + i;//将字符串放入arg中
			status = 1;//此时状态变为非空格状态1
		}
		else if(isspace(buf[i]))//如果遇到空格
		{
			buf[i] = 0;//使空格处为0，作为字符串的结束符
			status = 0;	//此时，状态变为空格状态0
		}
	}
	arg[count] = NULL;//提取完命令中的各字符后，使最后一个为NULL
}

//创建子进程
void do_fork()
{

	pid_t pid = fork();
	if(pid == -1)
	{
		perror("fork error");
		exit(1);
	}
	else if(pid == 0)//子进程
	{

	   int i = 0;
	   for(;i < count;i++)
	   {
		   if(strcmp(arg[i],"<") == 0)
		   {
			   close(1);//关闭标准输出即显示器文件
			   int fd = open(arg[i+1],O_CREAT|O_RDWR,0644);
			   if(fd < 0)
			   {
				   perror("open error");
				   exit(1);
			   }

			   printf("%d\n",fd);
			   arg[i] = NULL;
		   }
	   }
		int ret = execvp(arg[0],arg);//使子进程执行exec函数，arg[0]表示要执行的命令，arg表示以何种方式执行
		if(ret == -1)
		{
			printf("execv error\n");
			exit(1);
		}
	}
	else//父进程
	{
		int st;
		int ret1 = wait(&st);//父进程等待
		if(ret1 == -1)
		{
			printf("wait failed\n");
			exit(1);
		}

	}
}
int main()
{
	char buf[1024] = {};
	while(1)
	{
		printf("myshell# ");
		fgets(buf,sizeof(buf),stdin);//获取命令
		do_parse(buf);//解析命令
		do_fork();//创建子进程
	}
	return 0;
}
