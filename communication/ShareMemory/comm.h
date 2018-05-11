#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>

#define PATHNAME "."
#define PROJ_ID 0x6666

//异常退出宏函数
#define ERR_EXIT(m) \
    do\
    {\
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

//创建共享内存
int CreateShm(size_t size);
//获取共享内存
int GetShm(size_t size);
//挂接共享内存
int CatShm(int shmid,void* addr);
//断开共享内存
int DtShm(const void* addr);
//销毁共享内存
int DestoryShm(int shmid);
