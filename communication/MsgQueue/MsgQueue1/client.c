#include "comm.h"

int main()
{
    //获取消息队列
    int msgid = GetMsgQueue();

    char buf[1024] = {0};
    while(1)
    {
        printf("please enter#");
        fflush(stdout);
        ssize_t s = read(0,buf,sizeof(buf));
        if(s > 0)
        {
            buf[s - 1] = 0;
            //向消息队列中发送数据
            int ret1 = SendMsgQueue(msgid,CLIENT_TYPE,buf);
            if(ret1 < 0)//发送失败
            {
                break;
            }
            else
            {
                printf("send done\n");
            }
        }
        else
        {
            printf("read error\n");
            break;
        }


        //从消息队列中接受数据
        int ret_s = RcvMsgQueue(msgid,SERVER_TYPE,buf);
        if(ret_s < 0)
        {
            break;
        }
        else
        {
            printf("server say# %s\n",buf);
        }
    }
    return 0;
}
