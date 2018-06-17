#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<unistd.h>

void mycal(char* buf)
{
    printf("<html>\n");
    printf("<body>\n");

    int x;
    int y;
    sscanf(buf,"firstdata=%d&lastdata=%d",&x,&y);
    printf("<h3>%d + %d = %d</h3>\n",x,y,x+y);
    printf("<h3>%d - %d = %d</h3>\n",x,y,x-y);
    printf("<h3>%d * %d = %d</h3>\n",x,y,x*y);
    if(y == 0)
    {
        printf("<h3>%d / %d = %d(zero)</h3>\n",x,y,x/y);
        printf("<h3>%d % %d = %d(zero)</h3>\n",x,y,x%y);
    }
    else
    {
        printf("<h3>%d / %d = %d</h3>\n",x,y,x/y);
        printf("<h3>%d % %d = %d</h3>\n",x,y,x%y);
    }
    printf("</body>\n");
    printf("</html>\n");
}
int main()
{
    char buf[1024];
    //首先获取方法的环境变量
    char* method = NULL; 
    method = getenv("METHOD");
    if(method == NULL)
    {
        return 1;
    }
    if(strcasecmp(method,"GET") == 0)
    {
        sprintf(buf,"%s",getenv("QUERY_STRING"));
    }
    else if(strcasecmp(method,"POST") == 0)
    {
        int content_length = atoi(getenv("CONTENT_LENGTH")); 
        int i = 0;
        char c;
        for(;i < content_length;i++)
        {
            read(0,&c,1);
            buf[i] = c;
        }
    }

    mycal(buf);
    return 0;
}
