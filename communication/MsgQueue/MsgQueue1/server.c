#include "comm.h"

int main()
{
	//创建消息队列
	int msgid = CreateMsgQueue();

	char buf[1024] = {0};
	while(1)
	{
        //从消息队列中接受数据块
		int ret = RcvMsgQueue(msgid,CLIENT_TYPE,buf);
		if(ret < 0)//接收函数调用失败
		{
			break;
		}
		else//调用成功
		{
			printf("client say# %s\n",buf);//输出接受到的消息
		}

        //向消息队列中发送数据块
		printf("please enter#");
		fflush(stdout);
		ssize_t s = read(0,buf,sizeof(buf));//从键盘上读取信息
		if(s > 0)//读取成功
		{
			buf[s - 1] = 0;
            //向消息队列中发送数据
            int ret2 = SendMsgQueue(msgid,SERVER_TYPE,buf);
            if(ret2 < 0)//发送函数调用失败
            {
                break;
            }
            else//调用成功
            {
                printf("server send done\n");
            }
		}
		else//读取失败
		{
			printf("read error\n");
			break;
		}
	}

    //销毁消息队列
	DestoryMsgQueue(msgid);
	return 0;
}
