#include<stdio.h>
#include<unistd.h>
int main()
{
	int i = 0;
	for(;i<2;i++)
	{
		fork();
		printf("-\n");
	}
	wait(NULL);
	wait(NULL);
	return 0;
//	int i = 0;
//	for(;i<2;i++)
//	{
//		fork();
//		printf("-");
//	}
//	wait(NULL);
//	wait(NULL);
//	return 0;
}
