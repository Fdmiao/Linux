#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main()
{
    int fd = open("log.txt",O_CREAT|O_RDWR);
    if(fd < 0)
    {
        perror("open error");
        return 1;
    }

    close(1);
    dup2(fd,1);//dup2相对与dup的优势在于它可以自由选择新的没有用过的文件描述符
    //而dup是系统自动分配没有用过的最小的文件描述符
    //除此之外，其他点都与dup相同

    //此时还可以通过fd对普通文件进行写入
    write(fd,"hello",strlen("hello"));
    //不过因为已经重定向，一般都会将fd关闭
    close(fd);
    int i = 10;
    while(i--)
    {
        printf("hello world\n");
        fflush(stdout);
    }
    close(1);
    return 0;
}
