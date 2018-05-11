#include "comm.h"

//创建或访问一个信号量集
static int CommSem(int nsem,int semflags)
{
    key_t key = ftok(PATHNAME,PROJ_ID);//生成key值
    if(key < 0)//生成出错
    {
        ERR_EXIT("ftok error");
    }

    //创建或访问一个信号量集
    int semid = semget(key,nsem,semflags);
    if(semid < 0)//获取或访问出错
    {
        ERR_EXIT("semget error");
    }

    return semid;//返回创建或获取的信号量集标识符
}


//创建一个信号量集
int CreateSem(int nsem)
{
    return CommSem(nsem,IPC_CREAT|IPC_EXCL|0666);
}

//获取一个信号量集
int GetSem(int nsem)
{
    return CommSem(nsem,IPC_CREAT);
}

//初始化信号量集
int InitSem(int semid,int semnum,int value)
{
    union semun _un;//联合体变量
    _un.val = value;
    int ret = semctl(semid,semnum,SETVAL,_un);//初始化信号量集
    if(ret < 0)//初始化失败
    {
        printf("init error\n");
        return -1;
    }
    return 0;
}

//对信号量集进行P-V原语操作
static int CommOp(int semid,int who,int flag)
{
    struct sembuf sem;
    sem.sem_num = who;
    sem.sem_op = flag;
    sem.sem_flg = 0;
    int ret =  semop(semid,&sem,1);
    if(ret < 0)
    {
        printf("semop error\n");
        return -1;
    }
    return 0;
}

//对信号量集进行P操作
int P_Op(int semid,int who)
{
    return CommOp(semid,who,-1);
}

//对信号量集进行V操作
int V_Op(int semid,int who)
{
    return CommOp(semid,who,1);
}

//删除信号量集
int DestorySem(int semid,int nsem)
{
    int ret = semctl(semid,nsem,IPC_RMID);
    if(ret < 0)
    {
        printf("delete error\n");
        return -1;
    }
    return 0;
}
