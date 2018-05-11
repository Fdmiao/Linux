#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>


#define ROW 3
#define COL 3

//坐标信息
typedef struct
{
    int x;
    int y;
}location;

//客户端发送的进入游戏或退出游戏的信息
typedef struct Res
{
    int res;
}Response;

void menu();//打印菜单
void print(char arr[ROW][COL], int x, int y);//打印棋盘
void Init(char arr[ROW][COL], int x, int y);//初始化数组
//移动函数
void move(char arr[ROW][COL],int x,int y,location* loc,char flag);
int  iswin(char arr[ROW][COL], int x, int y);//判断是否有人赢或产生平局
int who_win(int ret);//判断是谁赢
