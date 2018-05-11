#include "comm.h"


//创建或获得消息队列
static int CommMsgQueue(int flags)
{
	key_t key = ftok(PATH_NAME,PROJ_ID);//获得key值
	if(key < 0)//key值获取失败
	{
		ERR_EXIT("ftok error");	
	}

	int msgid = msgget(key,flags);//创建或获取消息队列
	if(msgid < 0)//消息队列创建或获取失败
	{
		ERR_EXIT("msgget error");
	}

	return msgid;//返回创建或获取的消息队列标识符
}

//创建消息队列
int CreateMsgQueue()
{
	return CommMsgQueue(IPC_CREAT|IPC_EXCL|0666);//返回创建的消息队列的标识符
}

//获取消息队列
int GetMsgQueue()
{
	return CommMsgQueue(IPC_CREAT);//返回获取的消息队列的标识符
}

//销毁消息队列
int DestoryMsgQueue(int msgid)
{
	if(msgctl(msgid,IPC_RMID,NULL) < 0)
	{
		perror("msgctl error");
		return -1;
	}
	return 0;
}

//向消息队列中添加数据
int SendMsgQueue(int msgid,int who,char* msg)
{
	struct msgbuf buf;//定义数据块结构体
	buf.mtype = who;//给结构体赋值
	strcpy(buf.mtext,msg);
	
	if(msgsnd(msgid,(void*)&buf,sizeof(buf.mtext),0) < 0)//向消息队列中添加数据块
	{
		perror("msgsed error");//添加失败
		return -1;
	}
	return 0;
}
//在消息队列中接受数据
int RcvMsgQueue(int msgid,int who,char out[])
{

	struct msgbuf buf;
	if(msgrcv(msgid,(void*)&buf,sizeof(buf.mtext),who,0) < 0)
	{
		perror("msgrcv error");
		return -1;
	}
	strcpy(out,buf.mtext);
	return 0;
}
