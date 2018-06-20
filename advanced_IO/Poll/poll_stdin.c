#include<stdio.h>
#include<sys/poll.h>
#include<unistd.h>

int main()
{
    struct pollfd stdin_fd;
    stdin_fd.fd = 0;
    stdin_fd.events = POLLIN;

    while(1)
    {
        //利用poll来进行循环等待
        int ret = poll(&stdin_fd,1,1000);

        char buf[1024];
        //在规定的时间内有所关心的文件描述符上的事件就绪
        if(ret > 0 && (stdin_fd.events & POLLIN))
        {
            ssize_t s = read(0,buf,sizeof(buf) - 1);
            if(s > 0)
            {
                buf[s -1] = 0;
                printf("%s\n",buf);
            }
            else if(s == 0)
            {
                printf("close fd\n");
                close(0);
                continue;
            }
            else
            {
                printf("read error\n");
                close(0);
                continue;
            }
        }
        else if(ret == 0)
        {
            printf("timeout\n");
        }
        else
        {
            printf("poll error\n");
        }
    }
    return 0;
}
