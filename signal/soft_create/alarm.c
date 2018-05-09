#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

int count = 0;
void handler(int signo)
{
    printf("count %d\n",count);
    exit(1);
}
int main()
{
   // signal(SIGALRM,handler);
   // alarm(1);
   // while(1)
   // {
   //     count++;
   // }

//    alarm(1);
//    int count = 0;
//    while(1)
//    {
//        printf("count %d\n",count++);
//    }
//



    unsigned int sec = alarm(5);
    printf("hello world\n");
    sleep(1);
    sec = alarm(0);
    printf("%d\n",sec);
    return 0;
}
