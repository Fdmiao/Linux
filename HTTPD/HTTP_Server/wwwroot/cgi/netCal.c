#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MAX 1024

void mycal(char* buff)
{
	int x,y;
	sscanf(buff,"firstdata=%d&lastdata=%d",&x,&y);
	
	printf("<html>\n");
	printf("<body>\n");
	printf("<h3>%d + %d = %d</h3>\n",x,y,x+y);
	printf("<h3>%d - %d = %d</h3>\n",x,y,x-y);
	printf("<h3>%d * %d = %d</h3>\n",x,y,x*y);
	if(y == 0)
	{
		printf("<h3>%d / %d = %d (zero)</h3>\n",x,y,-1);
		printf("<h3>%d %% %d = %d (zero)</h3>\n",x,y,-1);
	}
	else
	{
		printf("<h3>%d / %d = %d</h3>\n",x,y,x/y);
		printf("<h3>%d %% %d = %d</h3>\n",x,y,x%y);
	}
	printf("</body>\n");
	printf("<html>\n");
}

int main()
{
	char buff[MAX] = {0};
	if(getenv("METHOD"))
	{
		if(strcasecmp(getenv("METHOD"),"GET") == 0)
		{
			strcpy(buff,getenv("QUERY_STRING"));
		}

		else
		{
			int i = 0;
			int content_length = atoi(getenv("CONTENT_LENGTH"));
			while(i < content_length)
			{
				char c;
				read(0,&c,1);
				buff[i] = c;
				++i;
			}
			buff[i] = '\0';
		}
	}
	//printf("%s\n",buff);
	mycal(buff);

	return 0;
}
