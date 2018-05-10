#include<stdio.h>
#include"add.h"
#include"sub.h"
int main()
{
	int a = 10;
	int b = 20;
	int ret1 = add(a,b);
	printf("a+b = %d\n",ret1);
	int ret2 = sub(a,b);
	printf("a-b = %d\n",ret2);
	return 0;
}
