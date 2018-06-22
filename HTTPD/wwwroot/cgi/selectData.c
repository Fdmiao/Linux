#include<stdio.h>
#include<mysql.h>

int main()
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

    for(i = 0;i < row;i++)
    {
        //获得每行的内容
        MYSQL_ROW rowdata = mysql_fetch_row(res);
        int j = 0;
        for(;j < col;j++)
        {
            printf("%s\t",rowdata[j]);
        }
        printf("\n");
    }
    mysql_close(mysql_fd);
    return 0;
}
