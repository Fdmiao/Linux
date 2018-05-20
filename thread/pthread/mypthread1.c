#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void* pthread1(void* arg)
{
    printf("i am phread1\n");
    int i = 10;

    while(i--)
    {
        sleep(1);
    }

    return (void*)1;
}

void* pthread2(void* arg)
{
    printf("i am phread2\n");
    pthread_exit((void*)2);
}

void* pthread3(void* arg)
{
    printf("i am phread3\n");
    while(1)
    {

        sleep(1);
    }
    return (void*)5;
}
int main()
{
    pthread_t tid1,tid2,tid3;
    void* ret;
    
    //如果一个线程退出后，之后创建的线程会在之前退出线程的内存空间上创建结构体
    //所以，每次输出的tid值相同，按下面注释方式创建的tid均相同
    //按下面没有注释的方式创建的线程tid不同
    printf("main tid : %x\n",pthread_self());
    
    pthread_create(&tid1,NULL,pthread1,NULL);
    //pthread_join(tid1,&ret);
    printf("pthread1 tid:%x return code: %d\n",tid1,(int)ret);

    pthread_create(&tid2,NULL,pthread2,NULL);
    //pthread_join(tid2,&ret);
    printf("pthread1 tid:%x return code: %d\n",tid2,(int)ret);

    pthread_create(&tid3,NULL,pthread3,NULL);
    pthread_cancel(tid3);
    //pthread_join(tid3,&ret);
    printf("pthread3 id:%x return code: %d\n",tid3,(int)ret);
    pthread_join(tid1,&ret);
    pthread_join(tid2,&ret);
    pthread_join(tid3,&ret);
    return 0;
}
