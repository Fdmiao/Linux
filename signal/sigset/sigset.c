#include<stdio.h>
#include<signal.h>
#include<unistd.h>

//打印未决信号集函数
void printpending(sigset_t pending)
{
    int i = 1;
    for(;i < 32;i++)
    {
        if(sigismember(&pending,i))//如果第i号信号为有效信号，则打印1，否则打印0
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
    }
    printf("\n");
}
int main()
{
    sigset_t pending;//设置信号集变量用于保存未决信号集
    sigset_t set,oset;//定义信号集变量，用于设置新屏蔽信号字和保存原来的信号屏蔽字
    sigemptyset(&set);//初始化信号集
    sigaddset(&set,2);//将2号信号设置为有效信号
    sigprocmask(SIG_SETMASK,&set,&oset);//屏蔽2号信号
    //sigprocmask(SIG_BLOCK,&set,&oset);
    int i = 1;
    while(1)
    {
        sigpending(&pending);//读取未决信号集，并保存在信号集变量pending中
        printpending(pending);//打印未决信号集pending
        sleep(1);
        if(i == 3)//5s后向本进程发送2号信号
        {
            raise(2);
        }
        if(i == 7)
        {
            sigprocmask(SIG_UNBLOCK,&set,NULL);//10s后解除对2号信号的屏蔽
            //sigprocmask(SIG_SETMASK,&oset,NULL);
        }
        i++;
    }
    return 0;

}
