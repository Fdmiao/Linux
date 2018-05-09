#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        printf("usage:%s signo\n",argv[0]);
        return 0;
    }
    int i = 10;
    while(i--)
    {
        printf("running\n");
        sleep(1);
        if(i == 5)
        {
            printf("will receive signo %d\n",atoi(argv[1]));
            sleep(1);
            int ret = raise(atoi(argv[1]));
            if(ret == -1)
            {
                perror("raise error");
                exit(1);
            }
            else
            {
                printf("receive signo %d\n",atoi(argv[1]));
            }
        }
    }
    return 0;
}
