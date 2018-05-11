#include "comm.h"

//创建/获取共享内存
static int CommShm(int size,int flags)
{
    //创建进程间通信标识符
    key_t key = ftok(PATHNAME,PROJ_ID);
    if(key < 0)//创建失败
    {
        ERR_EXIT("ftok error");
    }

    int shmid = shmget(key,size,flags);//创建或获取共享内存
    if(shmid < 0)//共享内存创建失败
    {
        ERR_EXIT("shmid error");
    }

    return shmid;//返回共享内存标识符
}

//创建共享内存
int CreateShm(size_t size)
{
    return CommShm(size,IPC_CREAT|IPC_EXCL|0666);
}

//获取共享内存
int GetShm(size_t size)
{
    return CommShm(size,IPC_CREAT);
}


//挂接共享内存
int CatShm(int shmid,void* addr)
{
    addr = shmat(shmid,NULL,0);
    if(addr != NULL)//挂解失败
    {
        printf("shmat error\n");
        return -1;
    }
    return 0;
}


//断开共享内存
int DtShm(const void* addr)
{
    int ret = shmdt(addr);
    if(ret < 0)//断开失败
    {
        printf("shmid error\n");
        return -1;
    }
    return 0;
}

//销毁共享内存
int DestoryShm(int shmid)
{
    int ret = shmctl(shmid,IPC_RMID,NULL);
    if(ret < 0)
    {
        printf("shmctl error\n");
        return -1;
    }
    return 0;
}
