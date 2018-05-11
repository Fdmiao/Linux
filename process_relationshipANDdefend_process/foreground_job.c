#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if(pid > 0)//father
    {
        int i = 5;//5s后父进程运行结束
        while(i)
        {
            i--;
            printf("i am father,pid : %d\n",getpid());
            sleep(1);
        }
    }
    else//child
    {
        while(1)//子进程一直运行
        {
            printf("i am child,pid: %d\n",getpid());
            sleep(1);
        }
    }
    return 0;
}
