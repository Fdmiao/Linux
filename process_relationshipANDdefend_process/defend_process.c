

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
#include<fcntl.h>
#include<string.h>

void mydaemon()
{
    umask(0);//子进程会继承父进程的文件掩码，为使守护进程对文件的读写具有最高权限，所以将掩码设置为0
    pid_t pid = fork();
    if(pid > 0)//father
    {
        exit(0);//父进程退出，子进程运行在后台，使shell认为该前台进程已经退出
        //但子进程并没有脱离原会话，原进程组，原控制终端
    }
    else if(pid < 0)
    {
        printf("fork error\n");
        exit(1);
    }

    //child
    setsid();//使子进程自成一个会话成为话首，一个进程组成为组长，断开与终端的联系
    //调用该函数的进程不能为进程组的组长进程，所以fork已经实现该需求


    //守护进程自成会话后，成为会话的组长，组长是可以重新申请控制终端的，所以未避免载申请控制终端
    //需要使该守护进程不再使会话组长，所以，需要在fork一次
    if(pid = fork() > 0)
    {
        exit(0);
    }
    else if(pid < 0)
    {
        printf("fork error\n");
        return;
    }

    //忽略SIGCHLD信号:当在守护进程创建子进程中，子进程退出时，如果不回收会形成僵尸进程，造成内存泄漏
    //如果父进程回收资源，会造成父进程的负担，所以，将SIGCHLD信号设置为SIG_ING，使子进程退出时，自动回收资源
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if(sigaction(SIGCHLD,&sa,NULL) < 0)
    {
        printf("sigaction error\n");
        return;
    }

    //将当前目录改为根目录,使守护进程拥有root用户的权限,因为根目录在admin目录之上
    //所以，要在/下运行，必须以root的权限，所以，./a.out时不会生成进程
    //但是若更改为一般的目录，admin用户就可以运行，因此可以生成进程
    if(chdir("/home/admin/bit_code") < 0)
    //if(chdir("/") < 0)
    {
        printf("child dir error\n");
        return;
    }

    //关闭不需要的文件描述符，因为
    //守护进程已经和控制终端脱离了，所以向标准输入中写并不会发送给守护进程，所以要关闭标准输入
    //在守护进程中利用printf打印消息时，并不会打印到控制终端上（标准输出，标准错误），
    //所以关闭标准输出，标准错误或将二者重定向到/dev/null，写入其中的内容直接丢弃
    close(0);
    close(1);
    close(2);
    //int fd0 = open("/dev/null",O_RDWR);
    //dup2(fd0,1);
    //dup2(fd0,2);
}

int main()
{
    mydaemon();
    //daemon(0,1);
    int fd = open("mytext.log",O_RDWR|O_CREAT,0666);
    if(fd < 0)
    {
        printf("open error\n");
        exit(1);
    }
    while(1)
    {
        write(fd,"hello\n",strlen("hello\n"));
        sleep(1);
    }
}
