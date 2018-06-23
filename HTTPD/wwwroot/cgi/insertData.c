#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<mysql.h>
#include<string.h>

//void insert_data(char* name,char* sex,char* phone)
//{
//    MYSQL *mysql_fd = mysql_init(NULL);
//    //一个IP向不同的端口号发送请求，此时可以在防火墙的角度屏蔽他
//    if(mysql_real_connect(mysql_fd,"127.0.0.1","root","yan21188*","31_class",3306,NULL,0) == NULL)
//    {
//        printf("connect failed!\n");
//        return;
//    }
//    printf("connect success!\n");
//    char sql[1024];
//    sprintf(sql,"insert into student_info(name,sex,phone) values(\"%s\",\"%s\",\"%s\")",name,sex,phone);
//    printf("sql:%s",sql);
//    //const char* sql = "insert into student_info(name,sex,phone) values(\"yanxuechun\",\"man\",\"18710600739\")";
//
//    mysql_query(mysql_fd,sql);
//
//
//    mysql_close(mysql_fd);
//}
//

//该函数为cgi程序
int main()
{
    printf("hehe\n");
    printf("mysql version:%s\n",mysql_get_client_info());
    
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
    printf("heheh\n");
//// 
  //
  //  char data[1024] = {0};
  //  if(getenv("METHOD"))
  //  {
  //      if(strcasecmp("GET",getenv("METHOD")) == 0)
  //      {
  //          strcpy(data,getenv("QUERY_STRING"));
  //      }
  //      else
  //      {
  //          int content_length = atoi(getenv("CONTENT_LENGTH"));
  //          int i = 0;
  //          for(;i < content_length;i++)
  //          {
  //              read(0,data+i,1);
  //          }
  //          data[i] = 0;
  //      }
  //  }
  //  
  // // printf("<html>\n");
  // // printf("<body>\n");
  //  printf("arg: %s\n",data);
  // // printf("</body>\n");
  // // printf("</html>\n");

  //  
  //  //name=""&sex=""&phone=""
 
  //  //为什么用指针，不用数组
  //  char *name;
  //  char *sex;
  //  char *phone;
  //  
  //  strtok(data,"=&");
  //  name = strtok(NULL,"=&");
  //  strtok(NULL,"=&");
  //  sex = strtok(NULL,"=&");
  //  strtok(NULL,"=&");
  //  phone = strtok(NULL,"=&");
  //  
  //  //sscanf(data,"name=%s&sex=%s&phone=%s",name,sex,phone);

  //  insert_data(name,sex,phone);
  //  
    return 0;
}


