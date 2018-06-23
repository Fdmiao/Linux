#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<mysql.h>


void selectData()
{
    MYSQL *mysql_fd = mysql_init(NULL);
    //一个IP向不同的端口号发送请求，此时可以在防火墙的角度屏蔽他
    if(mysql_real_connect(mysql_fd,"127.0.0.1","root","yan21188*","31_class",3306,NULL,0) == NULL)
    {
        printf("connect failed!\n");
        return;
    }
    printf("connect success!\n");
    char sql[1024];
    sprintf(sql,"select* from student_info");
    mysql_query(mysql_fd,sql);
    //得到查询结果
    MYSQL_RES* res = mysql_store_result(mysql_fd);
  //表中一共几行
    int row = mysql_num_rows(res);
    //表中一共几列
    int col = mysql_num_fields(res);
    //得到列名
    MYSQL_FIELD* field = mysql_fetch_fields(res);
    //打印列名
    int i = 0;
    for(;i < col;i++)
    {
        printf("%s\t",field[i].name);
    }
    printf("\n");


    printf("<table border=\"1\">");
    for(i = 0;i < row;i++)
    {
        //获得一行的数据
        MYSQL_ROW rowdata = mysql_fetch_row(res);
        int j = 0;
        printf("<tr>");
        //打印每行内容
        for(;j < col;j++)
        {
            printf("<td>%s</td>",rowdata[j]);
        }
        printf("</tr>");
    }
    printf("</table>");
    //断开与数据库的连接
    mysql_close(mysql_fd);

}

int main()
{
    char data[1024] = {0};
    if(getenv("METHOD"))
    {
        if(strcasecmp("GET",getenv("METHOD")) == 0)
        {
            strcpy(data,getenv("QUERY_STRING"));
        }
        else
        {
            int content_length = atoi(getenv("CONTENT_LENGTH"));
            int i = 0;
            for(;i < content_length;i++)
            {
                read(0,data+i,1);
            }
            data[i] = 0;
        }
    }
    printf("heheh\n");
    printf("arg: %s\n",data);

    //name=""&sex=""&phone=""
 
    //为什么用指针，不用数组
   // char *name;
   // char *sex;
   // char *phone;
   // 
   // strtok(data,"=&");
   // name = strtok(NULL,"=&");
   // strtok(NULL,"=&");
   // sex = strtok(NULL,"=&");
   // strtok(NULL,"=&");
   // phone = strtok(NULL,"=&");
   // 
    selectData();
    //sscanf(data,"name=%s&sex=%s&phone=%s",name,sex,phone);


    return 0;
}
