#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>

void SetNoBlock(int fd)
{
    //首先利用fcntl函数获取文件描述符的文件标记状态
    int flag = fcntl(fd,F_GETFL);
    if(flag < 0)
    {
        perror("fcntl");
        return;
    }

    //设置文件描述符的标记状态，在原有的基础上再加一个非阻塞的标记状态
    fcntl(fd,F_SETFL,flag | O_NONBLOCK);
    return;
}


int main()
{
    SetNoBlock(0);//将标准输入设置为非阻塞的状态
    //如果是非阻塞的状态，则没有数据可以读时，会干其他事情
    //如果是阻塞的状态，则没有数据可以读时，就会挂起阻塞，不能干其他的事情
    //当没有数据可读时,errno都会被设置为EAGAIN
    //阻塞状态时会被挂起等待，不会干其他的事情，直到有数据可读时
    //但非阻塞状态时，不会被挂起，仍然可以做其他的事情
    
    
    char buf[1024] = {0};
    while(1)
    {
        //注意：此处应使用int类型的变量来接收s，因为当s为-1时，如果是size_t类型，就会发生越界，不可能等于-1
        int s = read(0,buf,sizeof(buf) - 1);

        if(s > 0)
        {
            buf[s - 1] = 0;
            printf("%s\n",buf);
        }
        else if(s == 0)
        {
            printf("close fd\n");
            break;
        }
        //此时，s小于0，表示有两种情况，一种是读取失败，一种是没有数据可读时
        //如果是没有数据时，errno会被设置为EAGAIN
        else
        {
            printf("s:%d,errno:%d\n",s,errno);
            if(errno == EAGAIN)
            {
                printf("no data\n");//测试阻塞和非阻塞状态下该语句的输出情况
                sleep(1);
            }
            else
            {
                printf("error\n");
            }
        }
    }
    return 0;
}
