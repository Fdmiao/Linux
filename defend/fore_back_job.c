#include<stdio.h>
#include<unistd.h>

int main()
{
    pid_t pid;
    pid = fork();

    if(pid == 0)//child
    {
        while(1)
        {
            printf("i am a child\n");
            sleep(1);
        }
    }

    else if(pid > 0)//parent
    {
        int i = 0;
        while(1)
        {
            if(i == 5)
            {
                break;
            }
            printf("i am parent\n");
            i++;
            sleep(1);
        }
    }
    return 0;
}
