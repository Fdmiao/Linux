
long long  a;//对全局变量进行赋值，只有一条语句，是不是原子操作
#include<stdio.h>
int main()
{
    a = 5;
    return 0;
}
