#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>


void mydaemon()
{
    pid_t pid = fork();
    if(pid < 0)//创建子进程失败
    {
        perror("fork error");
        exit(1);
    }
    else if(pid > 0)
    {
        exit(0);//使父进程退出
    }

    //子进程调用setsid函数，创建新会话，新进程组，摆脱原控制终端
    if(setsid() < 0)
    {
        perror("setsid error");//setsid函数调用失败
        exit(2);
    }

    //为防止子进程再次申请连接控制终端，再次fork使父进程退出，子进程不再是话首进程
    if(pid = fork() > 0)
    {
        exit(0);
    }
    else if(pid < 0)
    {
        printf("fork again error\n");
        return;
    }

    //将子进程的掩码清空
    umask(0);

    //改变从父进程处继承的当前工作目录为根目录
    if(chdir("/") < 0)
    {
        printf("chdir error\n");
        return;
    }

    //将从父进程处继承的不再需要的文件关闭或重定向至/etc/null
    close(0);
    int fd0;
    if(fd0 = open("/dev/null",O_RDWR) < 0)
    {
        printf("open error\n");
        return;
    }
//    dup2(fd0,1);
    dup2(fd0,2);

    //将子进程的SIGCHLD处理信号设置为SIG_IGN
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGCHLD,&sa,NULL) < 0)
    {
        printf("sigaction error\n");
        return;
    }

}

int main()
{
    mydaemon();//创建守护进程

    //在守护进程打开一个文件
    int fd = open("mytext.log",O_CREAT|O_RDWR,0644);
    if(fd < 0)
    {
        printf("open error\n");
        exit(1);
    }

    //使守护进程1s中向上述打开的文件写一句hello
    while(1)
    {
        write(fd,"hello\n",strlen("hello\n"));
        sleep(1);
    }
    return 0;
}
