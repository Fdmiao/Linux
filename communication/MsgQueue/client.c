#include"comm.h"
int main()
{

	//获得消息队列
	int msgid = GetMsgQueue();
	
	char buf[1024] = {0};
	while(1)
	{
	//	buf[0] = 0;
		printf("please enter#");
		fflush(stdout);
		ssize_t s = read(0,buf,sizeof(buf));//先从键盘上获取消息
		if(s > 0)
		{
			buf[s-1] = 0;
			SendMsgQueue(msgid,CLIENT_TYPE,buf);//向消息队列中写入内容
			printf("client send done\n");
		}			
        RcvMsgQueue(msgid,SERVER_TYPE,buf);//从消息队列中获取server发送的消息
        printf("server say#%s\n",buf);

	}
	return 0;
}
