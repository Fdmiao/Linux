#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>

//新线程创建成功后执行的函数
void* run(void* arg)
{
    pid_t new_tid;
    new_tid = syscall(SYS_gettid);
    printf("new: tgid = %d, tid:%d\n",getpid(),new_tid);
   //while(1)
   //{
   //    printf("%s\n",(char*)arg);
   //    sleep(1);
   //}
}

int main()
{
    pid_t main_tid;
    main_tid = syscall(SYS_gettid);
    printf("main:tgid = %d, tid = %d\n",getpid(),main_tid);

    pthread_t tid;
    //printf("main pthread id:%d\n",pthread_self());
    
    //新线程创建成功后去执行run指向的函数，主线程接着执行后面的代码
    pthread_create(&tid,NULL,run,"i am new pthread");//主线程创建新线程
    sleep(1);
    //printf("main:new pthread id:%d\n",tid);
    //sleep(1);
   // while(1)//主线程要执行的代码
   // {
   //     printf("i am main pthread\n");
   //     sleep(1);
   // }
    return 0;
}
