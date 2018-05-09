#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
    printf("%d\n",getpid());
    while(1);
    return 0;
}
