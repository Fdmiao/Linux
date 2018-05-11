#include "comm.h"

int main()
{
    int shmid = GetShm(4096);//获取共享内存
    
    //挂接共享内存
    char* addr = shmat(shmid,NULL,0);
//    CatShm(shmid,addr);
   // sleep(2);

    int i = 0;
    while(i < 26)
    {
        addr[i] = 'A'+ i;
        i++;
        addr[i] = 0;
        sleep(1);

    }
    shmdt(addr);
   // DtShm(addr);
    sleep(1);
    return 0;
}
