#include<stdio.h>
#include<signal.h>

//打印信号屏蔽字
void printoset(sigset_t oset)
{
    int i = 1;
    for(;i < 32;i++)
    {
        if(sigismember(&oset,i))
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
    sigset_t oset;
    sigfillset(&oset);
    printoset(oset);
    while(1)
    {
        sigprocmask(SIG_UNBLOCK,NULL,&oset);
        printoset(oset);
        sleep(1);
    }
    return 0;
}
