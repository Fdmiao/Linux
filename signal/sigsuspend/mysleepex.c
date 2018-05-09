#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void handler(int signo)
{
    ;
}

unsigned int mysleep(unsigned int sec)
{
    //定义信号的捕捉
    struct sigaction act,oact;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigaction(SIGALRM,&act,&oact);//设置SIGALRM信号的捕捉函数，并保存原来的信号处理动作

    //屏蔽SIGALRM信号
    sigset_t set,oset;
    sigemptyset(&set);//初始化信号屏蔽字
    sigaddset(&set,SIGALRM);//将SIGALRM信号设置为有效信号
    //1.
    sigprocmask(SIG_SETMASK,&set,&oset);//屏蔽SIGALRM信号，并保存原来的信号屏蔽状态

    //设定闹钟
    alarm(sec);//sec秒之后向本进程发送SIGALRM信号

    //sigsuspend的作用：
    //1.根据参数来解除信号的屏蔽，
    //2.挂起进程，
    //3.信号递达返回后自动恢复解除信号前的状态
    sigset_t maskset = set;
    sigdelset(&maskset,SIGALRM);//解除SIGALRM信号的屏蔽
    sigsuspend(&maskset);//

    int ret = alarm(0);//取消闹钟

    //恢复1.处屏蔽SIGALRM前的状态
    sigprocmask(SIG_SETMASK,&oset,NULL);

    //恢复SIGALRM信号的原来处理动作
    sigaction(SIGALRM,&oact,NULL);

    //返回剩余的秒数
    return sec;
}


int main()
{
    printf("%d\n",getpid());
    while(1)
    {
        printf("hello world\n");
        mysleep(1);
    }
    return 0;
}
