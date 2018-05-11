#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

//实现1s输出一条消息，但sleep函数和printf输出函数不能位于同一执行流中
//分析：因为两个函数不能位于同一执行流，此时就需要有两个执行流即两个线程，一个用于printf，一个用于sleep
//      同时，要执行一次printf，在执行一次sleep，因此需要两个线程必须一个执行完了，执行另一个，不能一个执行流连续执行两次
//      此时，就需要线程进行同步，
//      因此，这里利用条件变量来实现线程间同步


//定义条件变量，用于使一个线程在指定条件下等待，然后切换到另一线程
//在另一线程中，条件满足后，切换到刚刚在等待中的线程
pthread_cond_t cond;
pthread_mutex_t mutex;//因为条件变量的使用必须用到互斥量

//在下面的两个线程中，由于调度器的选择，使sleep线程先运行

//该线程用于打印信息
void* print(void* arg)
{
    while(1)
    {
        printf("hello\n");
        pthread_cond_wait(&cond,&mutex);
        printf("i am pthread %d\n",(int)arg);
    }
}

//该线程用于sleep
void* mysleep(void* arg)
{
    while(1)
    {
        pthread_cond_signal(&cond);//signal之后先执行在其之后的语句，等到切换到另一线程后，在从wait处开始执行
        printf("i am pthread %d\n",(int)arg);
        sleep(1);
    }
}

//在上面两个线程中，sleep线程先运行，所以，会先输出1
//sleep时，切换到0线程，输出hello,然后wait进入等待，切换会1线程，sleep1s后，执行signal，wait被唤醒
//但是，还是会接着signal之后继续运行。所以1s后会输出1。执行sleep时，切换到0线程，接着wait继续运行，输出0,在输出hello.
//在进入wait，切换到1线程。1s后，signal使wait解锁，但会接着signal运行，输出1.进入sleep后，切换到0线程，输出0，hello,
//之后就这样循环运行

int main()
{
    //初始化条件变量,互斥量
    pthread_cond_init(&cond,NULL);
    pthread_mutex_init(&mutex,NULL);

    pthread_t tid[2];//定义两个用户级线程id
    
    //创建两个用户级线程
    pthread_create(&tid[0],NULL,print,(void*)0);
    pthread_create(&tid[1],NULL,mysleep,(void*)1);

    //主线程等待新线程
    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);

    //销毁条件变量，互斥量
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}


