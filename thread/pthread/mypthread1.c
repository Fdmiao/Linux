#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void* pthread1(void* arg)
{
    printf("i am phread1\n");
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
    pthread_t tid;
    void* ret;
    
    printf("main tid : %x\n",pthread_self());
    
    pthread_create(&tid,NULL,pthread1,NULL);
    pthread_join(tid,&ret);
    printf("pthread1 tid:%x return code: %d\n",tid,(int)ret);

    pthread_create(&tid,NULL,pthread2,NULL);
    pthread_join(tid,&ret);
    printf("pthread1 tid:%x return code: %d\n",tid,(int)ret);

    pthread_create(&tid,NULL,pthread3,NULL);
    pthread_cancel(tid);
    pthread_join(tid,&ret);
    printf("pthread3 id:%x return code: %d\n",tid,(int)ret);
    return 0;
}
