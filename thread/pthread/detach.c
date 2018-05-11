#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void* run(void* arg)
{
    pthread_detach(pthread_self());

    printf("hello\n");
    return (void*)1;
}

int main()
{
    pthread_t tid;
    void *ret;

    pthread_create(&tid,NULL,run,NULL);

   
    //sleep(1);
    //int r = pthread_join(tid,&ret);
    //1. 在新线程中自己分离
    //a)上面不加sleep时，在调用该函数时，主线程会阻塞等待新线程运行
    //  因为新线程自己已经分离，所以运行结束后，会自己回收资源，由join带回返回值，所以会等待成功
    //b)上面加sleep时，子线程会自己分离然后自己回收资源。当执行到join时，子进程已经退出，且不需要回收资源
    //   所以，会join失败

    //pthread_detach(tid);
    sleep(1);
    int r = pthread_join(tid,&ret);

    if(r == 0)
    {
        printf("wait success\n");
        printf("%d\n",(int)ret);
    }
    else
    {
        printf("wait fail\n");
    }
    return 0;
}
