#include "comm.h"
int main()
{
    //创建共享内存
    int shmid = CreateShm(4097);

    //挂解共享内存
    char* addr = shmat(shmid,NULL,0);
    //CatShm(shmid,(void*)addr);
    sleep(2);
    
    int i = 0;
    printf("client say#\n ");
    sleep(2);
    while(i < 26)
    {
        printf("%s\r",addr);
        i++;
        fflush(stdout);
        sleep(1);
    }
    printf("\n");

    
    //断开共享内存
   // DtShm(addr);
    shmdt(addr);

    sleep(2);
    //删除共享内存
    DestoryShm(shmid);
    return 0;
}
