#pragma once

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<string.h>
#include<unistd.h>

//异常退出宏函数
#define ERR_EXIT(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

#define PATH_NAME "/"
#define PROJ_ID 0x6666

#define SERVER_TYPE 1
#define CLIENT_TYPE 2

struct msgbuf
{
	long int mtype;
	char mtext[1024];
};

int CreateMsgQueue();
int GetMsgQueue();
int SendMsgQueue(int msgid,int who,char* msg);
int RcvMsgQueue(int msgid,int who,char out[]);
int DestoryMsgQueue(int msgid);
