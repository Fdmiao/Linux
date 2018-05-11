#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
void fun()
{
	printf("hjas\n");
	exit(1);
}
int main()
{
//	fun();


	printf("hello world");
	sleep(3);
	_exit(-1);
}
