#include<stdio.h>
#include<pthread.h>
#include<unistd.h>


int ticket = 100;

//对共享资源进行访问时，要申请互斥量对共享资源上锁
//即使在对共享资源进行操作时由于时间片的缘故切换到其他线程，因为该共享资源还被上个线程锁着，
//所以其他线程由于申请不到临界资源而挂起等待，最终切换回原线程后，共享资源的状态并没有被改变
//因此，原线程接着被切换的地方执行，从而保持了共享资源的原子性操作，并没有造成数据的不一致性
pthread_mutex_t mutex;

//注意：线程很多时，可能在程序中的任意位置被切换，所以要注意加usleep的位置从而达到想要的效果


void* buy_ticket(void* arg)
{
    while(1)//当还有票时，就一直买票。
    {
        pthread_mutex_lock(&mutex);//对共享资源上锁
        if(ticket > 0)
        {
            usleep(500);//当没有加互斥量时，用usleep来模拟线程很多的情况。
            //当前线程很多时，分配给每个线程的时间片有限，所以当执行完if语句后，可能被切换到其他线程
            //这里用usleep来模拟线程很多的情况，以达到切换到其他线程的目的,进而实现对共享资源的非原子行操作
            //来显示由于非原子行操作而造成的错误的信息
            printf("ticket remains %d\n",ticket);//当前还有余票多少张
            printf("pthread %d buy one\n",(int)arg);//线程arg买一张票
            ticket--;//买完后票数减1
            pthread_mutex_unlock(&mutex);//对共享资源解锁
        }
        else//票数为0，停止买票
        {
            pthread_mutex_unlock(&mutex);//对共享资源解锁
            break;
            //pthread_mutex_unlock(&mtex);//解锁语句也不能加在该处，会造成与加在if-else语句后相同的效果
            
        }

        //注意：线程很多时，可能在程序中的任意位置被切换，所以要注意加usleep的位置从而达到想要的效果
        usleep(500);//在加了互斥量后，上述if语句中中的usleep就看不到效果了
        //因为在上述usleep后，即使切换到其他线程，因为共享资源被锁着，其他线程也不能修改，而执行完if-else语句后
        //由于本例中，线程过少，留给本线程的时间片过多，所以一个线程就会将所有的票卖完
        //因此，要在这里加usleep，模拟测试线程很多时，在此处被切换给其他进程的情况


        //pthread_mutex_unlock;//如果将上述两个分支语句中的解锁操作合并在该处
        //若执行else语句时，会直接跳出while循环，线程结束，而互斥量还未解锁的问题，
        //再主线程中就会出现对未解锁的互斥量销毁的问题，从而造成阻塞
        //所以，解锁互斥来量的语句不能加在该处



        //usleep(50);//当没有加互斥量时，如果在这里加usleep，由于本例中，线程个数有限，线程可能只会在此处进行切换，
        //即进行完上述的if-else操作后才进行切换。从而显示不出由于对共享资源的非原子行操作而造成的错误的信息。
        //而实际在线程很多的情况下，在程序中的任意位置均可能被切换，
        //所以要考虑特殊的情况，即在if比较完后线程就切换的情况。
        //因此，在没有加互斥量时，要想显示错误信息，usleep就不能加在该处，而要加载if语句中。
    }
    return (void*)1;
}


int main()
{
    pthread_mutex_init(&mutex,NULL);//初始化互斥量
    pthread_t tid[4];//定义四个用户级线程id

    //创建四个线程模拟四个买票的人
    int i = 0;
    for(;i < 4;++i)
    {
        pthread_create(&tid[i],NULL,buy_ticket,(void*)i);
    }

    //主线程等待四个新线程退出
    for(i = 0;i < 4;++i)
    {
        pthread_join(tid[i],NULL);
    }

    pthread_mutex_destroy(&mutex);//销毁互斥量


    return 0;
}


