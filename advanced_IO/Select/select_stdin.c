
#include<stdio.h>
#include<sys/select.h>
#include<unistd.h>
int main()
{
    fd_set readfds;
    FD_ZERO(&readfds);//清空文件描述符集
    FD_SET(0,&readfds);//将标准输入设置进文件描述符集

    char buf[1024];
    while(1)
    {
        int ret = select(1,&readfds,NULL,NULL,NULL);//调用select关心所设置的文件描述符集
        if(ret > 0)//所关心的文件描述符集上至少有一文件描述符上的事件就绪
        {
            ssize_t s = read(0,buf,sizeof(buf) - 1);
            if(s > 0)
            {
                buf[s - 1] = 0;
                printf("%s\n",buf);
            }
            else
            {
                printf("error\n");
                continue;
            }
        }
        else if(ret == 0)
        {
            printf("timeout\n");
        }
        else
        {
            printf("error\n");
            continue;
        }
        FD_ZERO(&readfds);
        FD_SET(0,&readfds);
    }
    return 0;
}
