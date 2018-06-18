#include<stdio.h>
#include<stdlib.h>

int main()
{
    printf("echo Sever:%s\n",getenv("METHOD"));
    printf("echo Sever:%s\n",getenv("QUERY_STRING"));
    return 0;
}
