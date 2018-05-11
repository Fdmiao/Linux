#include "comm.h"


int main()
{
	int msgid = CreateMsgQueue();//创建消息队列

	char buf[1024] = {0};
	while(1)
	{
	//	buf[0] = 0;
		RcvMsgQueue(msgid,CLIENT_TYPE,buf);//接受client发送的消息
		printf("client say#%s\n",buf);
		
		//发送消息

		printf("please enter#");
		fflush(stdout);
		ssize_t s = read(0,buf,sizeof(buf));//从键盘中读取信息保存到buf中
		if(s > 0)
		{
            printf("%d\n",s);//在从键盘输入字符时，回车也算一个字符
			buf[s - 1] = 0;//当输入的字符个数等于buf的长度时，要使最大下标s-1处的元素为\0
			SendMsgQueue(msgid,SERVER_TYPE,buf);//向消息队列中发送消息
			printf("send done\n");
		}
	}
	DestoryMsgQueue(msgid);//销毁消息队列
	
	return 0;
}
