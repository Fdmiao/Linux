#include<stdio.h>
#include<mysql.h>
#include<string.h>
#include<stdlib.h>

void insert_data(char* name,char* sex,char* phone)
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
    sprintf(sql,"insert into student_info(name,sex,phone) values(\"%s\",\"%s\",\"%s\")",name,sex,phone);
    printf("sql:%s",sql);
    //const char* sql = "insert into student_info(name,sex,phone) values(\"yanxuechun\",\"man\",\"18710600739\")";

    mysql_query(mysql_fd,sql);


    mysql_close(mysql_fd);
}


//该函数为cgi程序
int main()
{
    //printf("mysql version:%s\n",mysql_get_client_info());
    
    char data[1024];
    if(getenv("METHOD"))
    {
        if(strcasecmp("GET",getenv("METHON")) == 0)
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
    printf("arg: %s\n",data);
    //name=""&sex=""&phone=""
 
    //为什么用指针，不用数组
    char *name;
    char *sex;
    char *phone;
    
    strtok(data,"=&");
    name = strtok(NULL,"=&");
    strtok(NULL,"=&");
    sex = strtok(NULL,"=&");
    strtok(NULL,"=&");
    phone = strtok(NULL,"=&");
    
    //sscanf(data,"name=%s&sex=%s&phone=%s",name,sex,phone);

    insert_data(name,sex,phone);
    
    return 0;
}
