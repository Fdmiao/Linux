#include<stdio.h>
#include<stdlib.h>
int main(int argc,char * argv[],char *env[])
{
	extern char **environ;
	int i=0;
	for(i=0;environ[i];i++)
	{
		printf("%s\n",environ[i]);
	}
//	int i=0;
//	for(;env[i];i++)
//	{
//		printf("%s\n",env[i]);
//	}

//	char *env=getenv("MYENV");
//	if(env)
//	{
//
//	printf("%s\n",env);
//	}
	return 0;
}
