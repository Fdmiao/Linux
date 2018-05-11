#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<unistd.h>
#include<sys/wait.h>

//异常退出宏函数
#define ERR_EXIT(m)\
    do\
    {\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

#define PATHNAME "."
#define PROJ_ID 0X6666

union semun
{

    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

//创建一个信号量集
int CreateSem(int nsem);
//获取一个信号量集
int GetSem(int nsem);
//初始化信号量集
int InitSem(int semid,int semnum,int value);
//对信号量集进行P操作
int P_Op(int semid,int who);
//对信号量集进行V操作
int V_Op(int semid,int who);
//删除信号量集
int DestorySem(int semid,int nsem);
