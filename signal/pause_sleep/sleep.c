#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>

void handler(int signo)
{
    printf("catch signo %d\n",signo);
}

int main()
{
    signal(11,handler);
    printf("pid %d\n",getpid());
    sleep(1);
    printf("hello world\n");
    int ret = sleep(20);//测试sleep函数的返回值，此时如没有到达20秒就被信号唤醒中断，会返回剩余的秒数
    printf("%d\n",ret);
    return 0;
}
