#include<stdio.h>
#include<string.h>
int main()
{
	const char* msg0 = "printf\n";
	const char* msg1 = "fwrite\n";
	const char* msg2 = "write\n";
	const char* msg3= "fprintf\n";

	printf("%s",msg0);
	fwrite(msg1,1,strlen(msg1),stdout);
	write(1,msg2,strlen(msg2));
	fprintf(stdout,"%s",msg3);

	fork();
	return 0;
}
