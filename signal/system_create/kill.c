#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        printf("usage:%s pid,signo\n",argv[0]);
        return 0;
    }
    int ret = kill(atoi(argv[1]),atoi(argv[2]));
    if(ret == -1)
    {
        perror("kill");
        exit(1);
    }
    else
    {
        printf("send %d signo to %d\n",atoi(argv[2]),atoi(argv[1]));
    }
    return 0;
}
