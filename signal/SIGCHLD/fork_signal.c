#include<stdio.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>

void handler(int signo)
{
    pid_t pid;
    while(pid = waitpid(-1,NULL,WNOHANG) > 0)
    {
        printf("child exit. child pid is %d\n",pid);

    }
}

int main()
{
    struct sigaction act,oact;
    act.sa_handler = SIG_IGN;//将SIGCHLD信号的处理动作设置为默认
    act.sa_flags = 0;
    //sigaction(SIGCHLD,&act,&oact);//对比该语句设置前后子进程退出后的状态变化
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
    {
        printf("i am child pid is %d\n",getpid());
        int count = 5;
        printf("hello world\n");
        sleep(2);
        exit(2);
    }

    printf("i am father\n");
    sleep(2);

    pid_t pid1 = fork();
    if(pid1 == 0)
    {
        printf("i am child2\n");
        exit(3);
    }

    printf("i am father\n");
    return 0;
}
