#pragma once

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<string.h>



#define PATHNAME "."
#define PROJ_ID 0x6666
#define SERVER_TYPE 1
#define CLIENT_TYPE 2

//异常退出宏函数
#define ERR_EXIT(m) \
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

struct msgbuf 
{
	long int mtype;//数据块的类型
	char mtext[1024];//数据快中的长度，柔性数组
};


//创建消息队列
int CreateMsgQueue();
//获取消息队列
int GetMsgQueue();
//向消息队列中添加数据块
int SendMsgQueue(int msgid,int type,char* msg);
//在消息队列中接收数据
int RcvMsgQueue(int msgid,int msgtyp,char out[]);
//销毁消息队列
int DestoryMsgQueue(int msgid);
