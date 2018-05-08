#include<stdio.h>
#include<unistd.h>
#include<signal.h>
//捕捉闹钟信号
void handler(int signo)
{
    ;
}
//捕捉其他信号
void handler1(int signo)
{
    printf("catch signo %d\n",signo);
}

int mysleep(int sec)
{
    struct sigaction act,oact;//act为要设置闹钟信号的相关信息，oact保存闹钟信号的原有相关信息
    act.sa_handler = handler;//自定义闹钟信号的处理动作
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);//在执行闹钟信号的处理动作时，不屏蔽其他信号
    sigaction(SIGALRM,&act,&oact);//捕捉闹钟信号

    alarm(sec);//sec秒后向进程发送个闹钟信号
    pause();//使进程挂起
    int ret = alarm(0);//取消闹钟，返回剩余的秒数
    sigaction(SIGALRM,&oact,NULL);//恢复闹钟信号的默认处理动作，验证有无该语句的对比
    return ret;
}


int main()
{
 //测试mysleep函数的功能   
//    while(1)
//    {
//        printf("hello world\n");
//        mysleep(1);
//    }
//    return 0;
//



//    signal(2,handler1);
//    printf("pid %d\n",getpid());
//    int ret = mysleep(20);//1s后键盘发送2号信号，验证sleep的返回值
//    printf("ret:%d\n",ret);
//    return 0;
//



//验证pause挂起过程中，其他信号也可以唤醒pause，所以会直接终止进程，不会输出最后一条语句
    printf("pid :%d\n",getpid());
    printf("hello world\n");
    mysleep(10);
    printf("pid :%d  will ingore SIGALRM\n",getpid());
    

}
