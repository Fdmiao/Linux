#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>

void mycal(char* buf)
{
    printf("<html>\n");
    printf("<body>\n");
    printf("arg:%s\n",buf);

    int x;
    int y;
    sscanf(buf,"firstdata=%d&lastdata=%d",&x,&y);
    printf("<h3>%d + %d = %d</h3>\n",x,y,x+y);
    printf("<h3>%d - %d = %d</h3>\n",x,y,x-y);
    printf("<h3>%d * %d = %d</h3>\n",x,y,x*y);
    if(y == 0)
    {
        printf("<h3>%d / %d = %d(zero)</h3>\n",x,y,x/y);
        printf("<h3>%d %% %d = %d(zero)</h3>\n",x,y,x%y);
    }
    else
    {
        printf("<h3>%d / %d = %d</h3>\n",x,y,x/y);
        printf("<h3>%d %% %d = %d</h3>\n",x,y,x%y);
    }
    printf("</body>\n");
    printf("</html>\n");
}
int main()
{
    printf("hehe\n");
    //之后写任何的cgi，开头都要写这些代码，将数据拿到
    //这些代码是固定套路
    char buf[1024] = {0};
    //首先获取方法的环境变量
    char* method = NULL; 
    method = getenv("METHOD");
    if(method == NULL)
    {
        return 1;
    }
    if(strcasecmp(method,"GET") == 0)//strcasecmp函数
    {
        strcpy(buf,getenv("QUERY_STRING"));
        //sprintf(buf,"%s",getenv("QUERY_STRING"));
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
    printf("arg:%s\n",buf);
    mycal(buf);

   // printf("echo Sever:%s\n",getenv("METHOD"));
   // printf("echo Sever:%s\n",getenv("QUERY_STRING"));
   // 
    return 0;
}
