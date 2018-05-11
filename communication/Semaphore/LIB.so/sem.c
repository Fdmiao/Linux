
#include "comm.h"

int main()
{
    //创建一个信号量集
    int semid = CreateSem(1);

    //初始化信号量集
    InitSem(semid,0,1);
    pid_t pid = fork();
    if(pid < 0)
    {
        ERR_EXIT("fork error");
    }
    else if(pid == 0)//子进程
    {
        while(1)
        {
            P_Op(semid,0);
            printf("A");
            fflush(stdout);
            usleep(123456);
            printf("A ");
            fflush(stdout);
            usleep(125455);
            V_Op(semid,0);
        }
    }
    else//父进程
    {
        while(1)
        {
            P_Op(semid,0);   
            printf("B");
            fflush(stdout);
            usleep(123456);
            printf("B ");
            fflush(stdout);
            usleep(125455);
            V_Op(semid,0);
        }
        wait(NULL);
    }
    DestorySem(semid,0);
    return 0;
}
