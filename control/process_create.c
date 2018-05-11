#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
char* arg[20]={};//定义全局变量arg，接收解析后的命令
int count =0;//定义全局变量count，接收实际的arg的个数
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

//func为子进程要执行的回调函数，参数为buf
void func(void* buf)
{
    char* buf1 = (char*)buf;//类型强转
	do_parse(buf1);//解析参数字符串
	int r = execvp(arg[0],arg);//子进程调用execvp函数去执行其他的程序
	if(r == -1)//execvp调用失败
	{
		perror("execvp");
		exit(1);
	}
}

////func为子进程要执行的回调函数，参数为buf
//void func(void* buf)
//{
//	printf("%s\n",(char*)buf);//使子进程输出参数字符串
//}

//该函数用于创建一个子进程，使子进程去执行回调函数func，buf为回调函数的参数
void process_create(pid_t* pid, void (*func)(void*), void* buf)
{
	pid_t ret1 = fork();//创建子进程
	if(ret1 == -1)//子进程创建失败
	{
		perror("fork");
		exit(1);
	}
	else if(ret1 > 0)//父进程
	{
		*pid = ret1;//*pid接受子进程的pid
		int status;
		int ret = wait(&status);//父进程等待
		if(ret == -1)//等待失败
		{
			perror("wait");
			exit(1);
		}
		else//等待成功
		{
			if(WIFEXITED(status))//子进程正常终止
			{
				printf("exit code %d\n",WEXITSTATUS(status));
			}
			else//子进程异常终止
			{
				printf("sig code %d\n",status & 0x7f);
			}
		}
	}
	else//子进程
	{
		(*func)(buf);//使创建好后的子进程去执行回调函数
		exit(0);//执行结束后，子进程终止
	}
}


//int main()
//{
//	pid_t pid = 0;//定义输出型变量，用于接收子进程的pid
//	char buf[100] = "hello world";//子进程要执行的回调函数的参数,在解析字符串时会改变字符串的内容，所以定义为数组的形式
//	process_create(&pid,func,buf);//通过创建子进程将字符串buf输出
//	printf("child:pid = %d\n",pid);
//	return 0;
//}
int main()
{
	pid_t pid = 0;//定义输出型变量，用于接收子进程的pid
	char buf[100] = "ls -a";//子进程要执行的回调函数的参数,在解析字符串时会改变字符串的内容，所以定义为数组的形式
	process_create(&pid,func,buf);//通过创建子进程去执行buf表示的shell命令
	printf("child:pid = %d\n",pid);
	return 0;
}

