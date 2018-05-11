#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

pthread_rwlock_t rwlock;
int count = 0;

void* mywrite(void* arg)
{
    while(1)
    {
        int counter = count;
        pthread_rwlock_wrlock(&rwlock);
        printf("before : %d.  %d write %d\n",counter,(int)arg,++count);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
}

void* myread(void* arg)
{
    while(1)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("%d read %d\n",(int)arg,count);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
}


int main()
{
    pthread_rwlock_init(&rwlock,NULL);
    pthread_t tid[8];

    int i = 0;
    for(;i < 3;i++)
    {
        pthread_create(&tid[i],NULL,mywrite,(void*)i);
    }

    for(i = 3;i < 8;i++)
    {
        pthread_create(&tid[i],NULL,myread,(void*)i);
    }

    for(i = 0;i < 8;i++)
    {
        pthread_join(tid[i],NULL);
    }

    pthread_rwlock_destroy(&rwlock);

    return 0;
}
