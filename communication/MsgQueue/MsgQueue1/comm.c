#include "comm.h"

//创建或获取消息队列
static int CommMsgQueue(int flags)
{
	key_t key = ftok(PATHNAME,PROJ_ID);//创建key值
	if(key < 0)//key值创建失败
	{
		ERR_EXIT("ftok error");
	}

	//key创建成功
	int msgid = msgget(key,flags);//创建或获取一个消息队列
	if(msgid < 0)//消息队列创建或获取失败
	{
		ERR_EXIT("msggid error");
	}

	return msgid;
}

//创建消息队列
int CreateMsgQueue()
{
	int msgid = CommMsgQueue(IPC_CREAT | IPC_EXCL | 0666);
	return msgid;
}

//获取消息队列
int GetMsgQueue()
{
	return CommMsgQueue(IPC_CREAT);
}

//向消息队列中发送数据
int SendMsgQueue(int msgid,int type,char* msg)
{
	struct msgbuf buf;
	buf.mtype = type;
	strcpy(buf.mtext,msg);
	int s = msgsnd(msgid,(void*)&buf,sizeof(buf.mtext),0);//向消息队列中发送数据
	if(s < 0)//数据发送失败
	{
		perror("msgsnd error");
		return -1;
	}
	return 0;
}

//在消息队列中接收数据
int RcvMsgQueue(int msgid,int msgtyp,char out[])
{
	struct msgbuf buf;
	int s = msgrcv(msgid,(void*)&buf,sizeof(buf.mtext),msgtyp,0);
	if(s < 0)
	{
		perror("msgrcv error");
		return -1;
	}
//	buf.mtext[s] = 0;
	strcpy(out,buf.mtext);
	return 0;
}

//销毁消息队列
int DestoryMsgQueue(int msgid)
{
	int ret = msgctl(msgid,IPC_RMID,NULL);
	if(ret < 0)
	{
		perror("msgctl error");
		return -1;
	}
	return 0;
}















