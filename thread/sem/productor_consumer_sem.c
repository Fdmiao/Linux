#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<stdlib.h>

//生产者消费者问题：用循环队列来做临界资源
//生产者与生产者之间是互斥关系，消费者与消费者之间是互斥关系，生产者与消费者之间是同步与互斥的关系
//互斥：通过互斥量来实现
//同步：循环队列为空时，只能生产者先运行。循环队列为满时，只能生产者先运行，
//      因此，在本问题中，分别用两个信号量来表示循环队列中空格的个数和满格的个数
//      当空格的个数为0即满格的个数为最大值n即循环队列为满时，消费者先运行
//      当满格的个数为0即空格的个数为最大个数n时，即循环队列为空时，生产者先运行
//      初始时，循环队列为空，所以，满格的个数为0，空格的个数为n
//在用信号量实现同步时，因为循环队列为空或为满时，生产者和消费者开始执行的下标位置相同。
//但是，上面的同步机制已经保证了某一个先运行，所以不会造成生产者与消费者之间的互斥问题
//也就是说，当队列不为空或不为满时，生产者和消费者都可以访问临界资源,且不会造成数据的不一致。
//因为只有为空或未满时，才可能访问同一下标处的数据，但这两种情况，在同步机制中已经实现只能一个先运行。所以不会发生互斥
//其余情况下，二者必定不会访问同一下标处的数据，所以，生产者与消费者之间不需要进行互斥量的设置
//在本问题中互斥量是用来保证生产者与生产者，消费者与消费者之间的互斥关系的
//当只有一个生产者和一个消费者,两个信号量时，是不用设置互斥量的。因为生产者与消费者之间的互斥关系已由同步机制实现

//在本问题中还可以用条件变量实现同步,不过需要设置两个条件变量，
//当队列为空时需要一个条件变量，当队列未满时，还需要设置一个条件变量


pthread_mutex_t mutex;
pthread_cond_t cond;
sem_t full;
sem_t empty;

//循环队列的结构
typedef struct  Queue
{
    int data[20];//循环队列有数组构成
    int size;//队列的实际长度
    int head;//从对头出队,head为队头元素所在的下标
    int tail;//从队尾入队，tail为队尾元素的下一个位置下标
}Queue;

Queue queue;//[head,tail)
//循环队列的初始化
void QueueInit()
{
    queue.size = 0;//初始时，循环队列为空，所以实际长度为0   
    queue.head = 0;
    queue.tail = 0;
}

//从队尾向队列中插入数据
void QueuePush()
{
    if(queue.size == 20)
    {
        //队列为满
        printf("队列已满\n");
        return;
    }
    int value = rand()%100 + 1;
    queue.data[queue.tail] = value;
    queue.tail = (queue.tail + 1) % 20;
    queue.size++;
}

//在队头删除元素
void QueuePop()
{
    if(queue.size == 0)
    {
        printf("队列已空\n");
        //空队列
        return;
    }
    queue.head = (queue.head + 1)%20;
    queue.size--;
}

//不加信号量时：模拟生产者比消费者优先级高时，可使生产者个数为20，消费者个数为3，并且生产者usleep(100),消费者usleep(2000000)
//不加信号量时：模拟生产者比消费者优先级低时，可使生产者个数为3，消费者个数为20，并且生产者usleep(1000000),消费者usleep(200)
//
//加了信号量后，也可已以用上述方法模拟上述两种情况，但是看不到现象，因为信号量为0时直接挂起，不为零时，直接执行，输出语句不易加入



//生产者生产产品
void* product(void* arg)
{
    while(1)
    {
        sem_wait(&empty);
        //首先：empty减1，即empty维护的临界资源个数减1
        //然后：如果empty值小于0，则将该线程挂起等待，使消费者先运行。否则什么也不做
        //如果empty不为0即队列不为满时，即还有临界资源，就申请empty维护临界资源，使临界资源的个数减1
        //如果empty为0即队列为满时，生产者必须挂起等待，使消费者先运行
        pthread_mutex_lock(&mutex);//生产者插入节点时要互斥的访问临界资源
        QueuePush();//插入节点
        int pre = 0;
        if(queue.tail == 0)
        {
            pre = 19;
        }
        else
        {
            pre = queue.tail - 1;
        }
        printf("%d product %d\n",(int)arg,queue.data[pre]);
        pthread_mutex_unlock(&mutex);//对临界资源操作完毕后，解锁互斥量，退出临界区
        sem_post(&full);
        //full加1，即full维护的临界资源个数加1，即元素个数加1
        //如果有消费者在挂起等待，则唤醒他。如果没有，则什么也不做
        usleep(1);
    }
}

//消费者消费产品
void* consume(void* arg)
{
    while(1)
    {
        sem_wait(&full);
        //首先：full减1，即full维护的临界资源个数减1
        //然后：如果full值小于1，则将该线程挂起等待，使生产者先运行。否则什么也不做
        //当full不为0即队列不为空时，就申请full维护的临界资源，使临界资源的个数减1
        //当full为0即队列为空时，消费者挂起等待，必须使生产者先运行
        pthread_mutex_lock(&mutex);//消费者删除节点时要互斥的访问临界资源
        printf("%d consume %d\n",(int)arg,queue.data[queue.head]);
        QueuePop();//删除节点
        pthread_mutex_unlock(&mutex);//对临界资源操作结束，解锁互斥量，退出临界区
        sem_post(&empty);
        //首先：empty加1，即empty维护的临界资源个数加1000000
        //然后：如果有生产者在挂起等待，则唤醒他。如果没有，则什么也不做
        usleep(1000000);
    }
}

int main()
{
    srand((unsigned int)time(NULL));
    pthread_mutex_init(&mutex,NULL);
    //pthread_cond_init(&cond,NULL);
 
    sem_init(&full,0,0);
    sem_init(&empty,0,20);

    pthread_t tid[8];

    int i = 0;
    for(;i < 4;++i)
    {
        pthread_create(&tid[i],NULL,product,(void*)i);
    }

    for(i = 4;i < 8;i++)
    {
        pthread_create(&tid[i],NULL,consume,(void*)i);
    }

    for(i = 0;i < 8;i++)
    {
        pthread_join(tid[i],NULL);
    }

    sem_destroy(&full);
    sem_destroy(&empty);
    return 0;
}
