#include<stdio.h>
#include<signal.h>
#include<unistd.h>

//自定义的信号处理函数
void handler(int num)
{
    printf("catch signo %d\n",num);
}

int main()
{
    
    signal(2,handler);//捕捉2号信号
    signal(3,handler);//捕捉3号信号
    while(1)
    {
        printf("hehe\n");
        sleep(1);
    }
    return 0;
}
