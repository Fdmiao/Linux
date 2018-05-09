#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void handler(int signo)
{
    int i = 4;
    while(i--)
    {
        printf("catch signo %d\n",signo);
        sleep(1);
    }
}

int main()
{
    struct sigaction act,oact;//act用来设置信号的相关信息，oact用来保存信号原有的相关信息
    act.sa_handler = handler;//将信号的处理动作设置为handler
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask,3);//对3号信号进行屏蔽 对比设置该语句前后，输入ctrl+c和ctrl+\的差别 
    sigaction(2,&act,&oact);//自定义2号信号的处理动作
    sigaction(3,&act,&oact);//自定义3号信号的处理动作
    int i = 1;
    while(1)
    {
        printf("hehe\n");
        sleep(1);
        if(i == 5)
        {
            sigaction(2,&oact,NULL);//5s后恢复2号信号的原处理动作
            oact.sa_handler = SIG_IGN;//5s后将3号信号的处理动作设置为忽略
            sigaction(3,&oact,NULL);
        }
        i++;
    }

    return 0;
}
