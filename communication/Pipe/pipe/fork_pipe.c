#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

//内核中宏函数的写法
//因为要执行多语句，所以要用do-while循环语句进行封装，但只执行一次
#define ERR_EXIT(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

//在宏函数中，只执行一条语句，可以不用do-while循环语句
#define SUCC_EXIT() exit(EXIT_SUCCESS)

//在宏函数中，只执行一条语句，也可以用do-while循环语句
#define SUCC_EXIT() exit(EXIT_SUCCESS)
//#define SUCC_EXIT()\
//	do\
//	{\
//		exit(EXIT_SUCCESS);\
//	}while(0)

int main()
{
	int pipe_fd[2];
	char *msg = "hello world\n";
	if(pipe(pipe_fd) == -1)//管道创建失败
	{
		ERR_EXIT("pipe error");//调用宏函数
//		perror("pipe error");
//		exit(EXIT_FAILURE);
	}

	pid_t pid;
	pid = fork();//创建子进程
	if(pid < 0)//子进程创建失败
	{
		ERR_EXIT("fork error");//调用宏函数
//		perror("fork error");
//		exit(EXIT_FAILURE);
	}
    
	if(pid == 0)//子进程
	{
		close(pipe_fd[0]);//关闭子进程的读端
		write(pipe_fd[1],msg,strlen(msg));//子进程往管道中写入内容
		SUCC_EXIT();//调用宏函数,写完后，子进程成功退出
    }
	//父进程
	close(pipe_fd[1]);//关闭写端
	char buf[100] = {0};
	int s = read(pipe_fd[0],buf,sizeof(buf));//父进程从管道中读取内容
	if(s > 0)
	{
        buf[s] = 0;
	}
	printf("%s",buf);//输出读到的内容
    return 0;



//	//测试读端关闭，写端一直写的情况
//	if(pid == 0)
//	{
//		close(pipe_fd[0]);//关闭子进程的读端
//		while(1)
//		{
//			write(pipe_fd[1],msg,strlen(msg));//子进程往管道中写入内容
//		}
//		printf("child still running\n");//读端关闭后，写端一直写，write操作会产生信号SIGPIPE，使write进程退出，所以该语句不会输出
//	}
//	close(pipe_fd[1]);//关闭写端
//	char buf[100] = {0};
//	int s = read(pipe_fd[0],buf,strlen(msg));//父进程从管道中读取内容
//	printf("%s",buf);//输出读到的内容
//	s = read(pipe_fd[0],buf,strlen(msg));//父进程从管道中读取内容
//	printf("%s",buf);//输出读到的内容
//	close(pipe_fd[0]);//读完后关闭读端
//	return 0;




	//测试写端关闭时，读端一直读，直到返回0的情况
	
//	if(pid == 0)//子进程
//	{
//		close(pipe_fd[0]);//关闭子进程的读端
//		write(pipe_fd[1],msg,strlen(msg));//子进程往管道中写入内容
//		close(pipe_fd[1]);//写完后，关闭写端
//		SUCC_EXIT();//调用宏函数
//		//exit(EXIT_SUCCESS);//子进程退出
//	}
//
//	//父进程
//	close(pipe_fd[1]);//关闭写端
//	char buf[100] = {0};
//	int s = read(pipe_fd[0],buf,strlen(msg)-2);//父进程从管道中读取内容
//	//if(s > 0)
//	//{
//		//buf[s] = 0;
//	//}
//	printf("%s",buf);//输出读到的内容
//	s = read(pipe_fd[0],buf,strlen(msg));//父进程从管道中读取内容
//	printf("%s",buf);//输出读到的内容
//	s = read(pipe_fd[0],buf,strlen(msg));//父进程从管道中读取内容
//	printf("%d\n",s);//写端关闭，读端没有内容可读时，read会返回0
//	return 0;
}
