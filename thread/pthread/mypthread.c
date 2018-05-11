#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<stdlib.h>

void* pthread_run(void* msg)
{
    printf("%s\n",(char*)msg);
    pid_t pid;
    pid = syscall(SYS_gettid);
    printf("new pthread(kernal) pid:%d\n",pid);

    printf("new pthread(usr) tid:%d\n",pthread_self());
    while(1)
    {
        printf("hello \n");
        sleep(1);
    }

    //以下两种方法均可以使新线程退出
    return (void*)1;
    //pthread_exit((void*)273);

    //pthread_cancel(pthread_self());
    //sleep(1);
    //printf("hello world\n");
    //可以用该函数自己取消自己，但要加sleep或printf语句进行缓冲，因为执行退出函数需要时间，加完后，新线程的退出码为-1
    //return (void*)123;
}

int main()
{
    //得到内核主线程即轻量级进程的PID
    pid_t pid;
    pid = syscall(SYS_gettid);
    printf("main pthread(kernal) pid:%d\n",pid);

    //得到用户级主线程的PID
    printf("main pthread(usr) tid:%d\n",pthread_self());

    //得到用户级新线程的PID
    pthread_t tid;
    char* msg = "hello world";
    int ret = pthread_create(&tid,NULL,pthread_run,msg);
    if(ret != 0)
    {
        perror("pthread_create error");
        exit(1);
    }


//    sleep(1);
//    pthread_exit((void*)345);//可以使主线程退出，但新线程不会退出，所以后面的语句不会输出
//    printf("as");
//
   // sleep(1);
    //在取消线程之间加sleep，要取消的线程可能已经在取消之前自己退出了，此时，该线程的退出码为自己退出时的退出码
    //如果不加sleep，则新线程可能还没运行完就被取消了，所以线程的退出码为-1
    //pthread_cancel(tid);//取消某个线程时，该线程的退出码为-1

    //获取新线程的退出码
    void* status;
    pthread_join(tid,&status);
    printf("new pthread exit status:%d\n",(int)status);

    //pthread_exit((void*)345);//可以使主线程退出，但进程退出码不为该函数指定的退出码

    pthread_cancel(pthread_self());
    printf("jscb\n");
    ////sleep(1);
    //printf("hello world\n");

    //sleep(1);
    //如果主线程不sleep 1秒，主线程先运行后，会直接运行结束，然后进程终止，子线程还来不及运行
    //此时会会造成内存泄漏，所以要进行线程等待,来回收新线程资源并获取新线程退出时的状态
    return 0;
}



