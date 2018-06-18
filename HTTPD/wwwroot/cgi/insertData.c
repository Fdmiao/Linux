#include<stdio.h>
#include<mysql.h>

int main()
{
    printf("mysql version:%s\n",mysql_get_client_info());
    return 0;
}
