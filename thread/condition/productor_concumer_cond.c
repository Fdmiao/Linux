#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

//利用带头节点的单链表作为临界资源，实现生产者与消费者模型
//生产者与生产者之间是互斥关系
//消费者与消费者之间是互斥关系
//生产者与消费者之间是同步与互斥关系：
//  互斥：生产者生产的时候不能消费，消费值消费的时候不能生产
//  同步：没有产品的时候，生产者必须先运行。产品满的时候，消费者必须先执行
//在本例中，因为链表为临界资源，链表没有满的时候，所以只考虑链表为空的时候，即只需要一个条件变量来保证同步


pthread_mutex_t mutex;
pthread_cond_t cond; 

//定义单链表节点的结构
typedef struct LinkListNode
{
    int data;//数据域
    struct LinkListNode* next;//指向下个节点的指针域
}LinkListNode;

LinkListNode* head;

//创建节点
LinkListNode* CreateNode(int value)
{
    LinkListNode* new_node = (LinkListNode*)malloc(sizeof(LinkListNode));
    new_node->next = NULL;
    new_node->data = value;
    return new_node;
}

//初始化带头节点的单链表
void LinkListInit()
{
    head = CreateNode(0);
}


//头插节点
void LinkListPushFront()
{
    if(head == NULL)
    {
        //非法输入
        return;
    }
    int value = rand()%100 + 1;
    LinkListNode* new_node = CreateNode(value);
    new_node->next = head;
    head = new_node;
    return;
}

//头删节点
void LinkListPopFront()
{
    if(head == NULL)
    {
        //非法输入
        return;
    }
    if(head->next == NULL)
    {
        //空链表
        return;
    }

    LinkListNode* delete = head->next;
    head->next = delete->next;
    free(delete);
}

////消费者进行头删节点
//void* consume(void* arg)
//{
//    while(1)
//    {
//        pthread_mutex_lock(&mutex);
//        while(head->next == NULL)
//        //若while为if，可能会被异常唤醒。唤醒后，可能还是没有数据，就会对空链表进行操作，从而非法访问内存
//        {
//            printf("空链表\n");
//            pthread_cond_wait(&cond,&mutex);
//        }
//        printf("%d consume %d\n",(int)arg,head->next->data);
//        LinkListPopFront();
//        pthread_mutex_unlock(&mutex);
//        usleep(5000);
//    }
//}
//
////生产者进行头插节点
//void* product(void* arg)
//{
//    while(1)
//    {
//        pthread_mutex_lock(&mutex);
//        LinkListPushFront();
//        printf("%d product %d\n",(int)arg,head->next->data);
//        pthread_cond_signal(&cond);
//        pthread_mutex_unlock(&mutex);
//        usleep(1000000);
//    }
//}
//
//int main()
//{
//    LinkListInit();
//    pthread_mutex_init(&mutex,NULL);
//    pthread_cond_init(&cond,NULL);
//    srand((unsigned int)time(NULL));
//    pthread_t tid[8];
//
//    int i = 0;
//    for(;i < 3;++i)
//    {
//        pthread_create(&tid[i],NULL,product,(void*)i);
//    }
//
//    for(i = 3;i < 8;++i)
//    {
//        pthread_create(&tid[i],NULL,consume,(void*)i);
//    }
//
//    for(i = 0;i < 8;i++)
//    {
//        pthread_join(tid[i],NULL);
//    }
//
//    pthread_mutex_destroy(&mutex);
//    pthread_cond_destroy(&cond);
//
//    return 0;
//}
//

//消费者进行头删节点
void* consume(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(head->next == NULL)
        //若while为if，可能会被异常唤醒。唤醒后，可能还是没有数据，就会对空链表进行操作，从而非法访问内存
        {
            printf("空链表\n");
            pthread_cond_wait(&cond,&mutex);
        }
        printf("%d consume %d\n",(int)arg,head->next->data);
        LinkListPopFront();
        pthread_mutex_unlock(&mutex);
        usleep(5000);
    }
}

//生产者进行头插节点
void* product(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        LinkListPushFront();
        printf("%d product %d\n",(int)arg,head->next->data);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        usleep(1000000);
    }
}

int main()
{
    LinkListInit();
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    srand((unsigned int)time(NULL));
    pthread_t tid[8];

    int i = 0;
    for(;i < 3;++i)
    {
        pthread_create(&tid[i],NULL,product,(void*)i);
    }

    for(i = 3;i < 8;++i)
    {
        pthread_create(&tid[i],NULL,consume,(void*)i);
    }

    for(i = 0;i < 8;i++)
    {
        pthread_join(tid[i],NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}




