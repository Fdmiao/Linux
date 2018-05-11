
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
            P_Op(semid,0);//子进程申请显示器资源，如果没有，则等待，如果有，则使用，此时其他进程不能使用
            printf("A");//向显示器上输出A
            fflush(stdout);
            usleep(123456);
            printf("A ");//在输出A
            fflush(stdout);
            usleep(125455);
            V_Op(semid,0);//归还显示器资源，归还后，其他进程才可使用
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
    DestorySem(semid,0);//删除信号量集
    return 0;
}
//
//int main()
//{
//
//    pid_t pid = fork();
//    if(pid < 0)
//    {
//        ERR_EXIT("fork error");
//    }
//    else if(pid == 0)//子进程
//    {
//        while(1)
//        {
//            printf("A");
//            fflush(stdout);
//            usleep(123456);
//            printf("A ");
//            fflush(stdout);
//            usleep(125455);
//        }
//    }
//    else//父进程
//    {
//        while(1)
//        {
//            printf("B");
//            fflush(stdout);
//            usleep(123456);
//            printf("B ");
//            fflush(stdout);
//            usleep(125455);
//        }
//        wait(NULL);
//    }
//    return 0;
//}
