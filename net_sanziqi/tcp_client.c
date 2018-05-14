
#include"comm.h"

void client_work(int client_sock)
{
    int ret = 0;
    char arr[ROW][COL] = { 0 };
    Init(arr, ROW, COL);//初始化数组
    print(arr, ROW, COL);//首先打印九宫格
    while (1)
    {
        location loc;
        printf("玩家2走，玩家2请选择：");
        move(arr,ROW,COL,&loc,'*');
        write(client_sock,&loc,sizeof(loc));
        print(arr, ROW, COL);
        ret = iswin(arr, ROW, COL);
        int r = who_win(ret);//判断是谁赢
        if(r == 1)
        {
            //有人赢
            break;
        }
        printf("wait ...\n");

        location client_loc;
        ssize_t s = read(client_sock,&client_loc,sizeof(client_loc));

        arr[client_loc.x - 1][client_loc.y -1] ='Y';

        print(arr,ROW,COL);
        ret = iswin(arr,ROW,COL);
        r = who_win(ret);//判断是谁赢
        if(r == 1)
        {
            //有人赢
            break;
        }
    } 
}
int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("Usage:%s [ip][port]\n",argv[0]);
        return 1;
    }
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        printf("socket error\n");
        return 2;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    socklen_t len = sizeof(server);

    if(connect(sock,(struct sockaddr*)&server,len) < 0)
    {

        printf("connect failed\n");
        return 3;
    }

    printf("connect success\n");


    int input = 0;
    Response res;
    do
    {
        menu();
        printf("请选择：");
        scanf("%d", &input);
        switch (input)
        {
            case 1:
                printf("开始游戏\n");
                res.res = input;
                write(sock,&res,sizeof(res));
                client_work(sock);
                printf("\n");
                break;
            case 0:
                res.res = input;
                write(sock,&res,sizeof(res));
                printf("退出游戏\n");
                break;
            default:
                printf("选择错误，请重新选择！\n");
                printf("\n");
                break;
        }

    } while (input);
    close(sock);
    return 0;
}
