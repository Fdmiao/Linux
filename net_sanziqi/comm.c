#include "comm.h"

void Init(char arr[ROW][COL], int x, int y)//数组的初始化函数定义
{
    memset(arr, ' ', ROW*COL*sizeof(arr[0][0]));
}

void menu()//菜单函数定义
{
    printf("*****************************\n");
    printf("****1.开始游戏 0.退出游戏****\n");
    printf("*****************************\n");
}

void print(char arr[ROW][COL], int x, int y)//打印棋盘函数定义
{
    int i = 0;
    for (i = 0; i < ROW; i++)
    {
        printf(" %c  |  %c  |  %c \n", arr[i][0], arr[i][1], arr[i][2]);
        if (i < ROW - 1)
        {
            printf("---- ---- ----\n");
        }
    }
}

int who_win(int ret)//判断是谁赢
{
    if (ret == 'Y')
    {
        printf("玩家1赢\n");
        return 1;
    }
    else if (ret == '*')
    {
        printf("玩家2赢\n");
        return 1;
    }
    else if (ret == 0)
    {
        printf("平局\n");
        return 1;
    }
    return 0;
}
int  iswin(char arr[ROW][COL], int x, int y)
{
    int i = 0;
    int j = 0;
    int count = 0;
    if ((arr[0][0] == arr[1][1]) && (arr[1][1] == arr[2][2]))//正对角赢
    {
        return arr[0][0];
    }
    if ((arr[0][2] == arr[1][1]) && (arr[1][1] == arr[2][0]))//玩家斜对角赢
    {
        return arr[0][2];
    }
    for (i = 0; i < x; i++)
    {
        if ((arr[i][0] == arr[i][1]) && (arr[i][1] == arr[i][2]))
        {
            return arr[i][0];//横向赢
        }
    }
    for (i = 0; i < y; i++)
    {
        if ((arr[0][i] == arr[1][i]) && (arr[1][i] == arr[2][i]))
        {
            return arr[0][i];//竖向赢
        }
    }
    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            if (arr[i][j] == ' ')
            {
                count++;//统计棋盘上空格的个数，如果没有空格表示棋盘满了还没分出胜负，则平局
            }
        }
    }
    if (count == 0)
    {
        return 0;
    }

}

//移动函数
void move(char arr[ROW][COL],int x,int y,location* loc,char flag)
{
    int count = 0;
    while(!count)
    {
        scanf("%d%d",&(loc->x),&(loc->y));//服务器走
        if (loc->x >= 1 && loc->x <= ROW && loc->y >= 1 && loc->y <= COL)
        {
            if (arr[loc->x - 1][loc->y - 1] == ' ')
            {
                arr[loc->x - 1][loc->y - 1] = flag;
                count++;
            }
            else
            {
                printf("该位置已满！请重新选择：");
            }
        }
        else
        {
            printf("位置错误！请重新选择：");
        }
    }
}
