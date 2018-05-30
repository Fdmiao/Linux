
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

//将标准输出重定向到文件中
int main()
{
//    //传统的方法：
//    //1. 先关闭标准输出的文件描述符（标准输出的文件描述符为1）
//    //2. 再打开一个文件（此时该文件的文件描述符从最小值开始分配），所以必定为1
//    //3. 因为FILE结构体中封装了文件描述符，所以结构体变量：标准输出stdout中封装了文件描述符1
//    //4. 利用printf输出时，其实是往标准输出stdout中输出，即最终找的是文件描述符1
//    //5. 在关闭文件描述符1之后，printf还是往stdout中输出，找的还是文件描述符1
//    //   不过此时，文件描述符1处对应的文件指针已经变成新打开的普通文件的指针
//    //   所以，printf在输出时，其实是往新打开的文件中输出
//    close(1);
//    //此时fd必为1
//    int fd = open("log.txt",O_CREAT|O_RDWR);
//    if(fd < 0)//调用open时，如果文件已经存在，则会调用失败
//    {
//        perror("open");
//        return 1;
//    }
//
//    printf("%d\n",fd);
//
//
//    int i = 10;
//    while(i--)
//    {
//        printf("hello world\n");
//        fflush(stdout);
//    }
//    close(fd);


//    //如果在打开新的文件描述符之后，在关闭标准输出，此时要将标准输出重定向到新的文件
//    //就需要先关闭标准输出，再关闭新的文件描述符，在打开新的文件描述符，这样过于麻烦
//    //而且在新文件第一次创建之后就已经存在了，如果再次打开的话会打开失败
//    int fd = open("log.txt",O_CREAT|O_RDWR);
//    close(1);
//    close(fd);
//    int fd1 = open("log.txt",O_CREAT|O_RDWR);
//    if(fd1 < 0)
//    {
//        perror("open");
//        return 1;
//    }
//    printf("%d\n",fd1);
//    printf("bcsj\n");
//    fflush(stdout);
//


    //使用dup函数实现文件描述符的重定向:适用于在打开新的文件之后在关闭标准输出的文件描述符
    //1. 首先打开一个新的文件
    //2. 再关闭标准输出的文件描述符
    //3. 将标准输出重定向到文件中，利用dup函数实现文件描述符的重定向
    //   此时dup会为旧文件描述符分配一个未使用的最小的数组下标（文件描述符）
    //   因为标准输出关闭了，所以新的文件描述符一定是1
    //   即将旧的文件描述符对应的文件指针赋值一份到新的文件描述符中
    //   此时，就完成了重定向，即将新的文件描述符重定向到打开的文件中，即将标准输出重定向到文件中
    int fd = open("log.txt",O_CREAT|O_RDWR);
    if(fd < 0)
    {
        perror("open error");
        return 1;
    }

    close(1);
    //注意：新文件描述符的内容是旧文件描述符内容的一份拷贝
    int new_fd = dup(fd);
    if(new_fd < 0)
    {
        perror("dup");
        return 2;
    }
    close(fd);//因为文件已经重定向了，所以旧的文件描述符也就没有用了
    //如果该文件描述符不关闭，也可对该文件进行读写
    printf("new_fd:%d\n",new_fd);
    write(fd,"hello",strlen("hello"));
    
    int i = 10;
    while(i--)
    {
        //printf重定向到普通文件，普通文件是全缓冲，
        //即printf输出的数据会先保存在stdout结构体的缓冲区中
        //等待程序结束才会刷新缓冲区，输出到文件中
        //但是在本程序中，因为在程序结束之前已经close了文件描述符
        //即在程序结束后找不到文件了，所以在close之前将缓冲区中的内容刷新到文件中
        //所以要调用fflush来刷新缓冲区
        //
        //如果在程序结束之前没有close文件，也可以不用fflush刷新，程序结束后会自动刷新到缓冲区中
        //但是为防止其他特殊情况，还是刷新一下比较好
        printf("hello world\n");//printf重定向到普通文件，
        fflush(stdout);
    }
    close(new_fd);
    return 0;

}
