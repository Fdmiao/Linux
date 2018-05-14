#pragma once

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
//将两个加数和被加数封装为结构体
typedef struct 
{
    int x;
    int y;
}Request;

//加法计算的结果
typedef struct
{
    int sum;
}Response;
