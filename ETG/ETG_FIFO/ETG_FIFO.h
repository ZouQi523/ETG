//
//  ETG_FIFO.h
//  ETG
//
//  Created by 邹琪 on 2019/8/23.
//  Copyright © 2019 邹琪. All rights reserved.
//

#ifndef ETG_FIFO_h
#define ETG_FIFO_h

#include <stdio.h>
#define bool int
#define true 1
#define false 0
//使能FIFO配套的字符串解析功能
#define  ENALBE_ETG_FIFO_STR
//使能测试函数
#define ENALBE_ETG_FIFO_TEST

typedef struct __ETG_FIFO_Struct
{
    //用户需写入的配置项目
    bool enalbeLoopWrite;
    unsigned char *buffer;
    unsigned int bufferSize;
    //
    bool isFull;
    bool writeLock;
    unsigned int readNum;
    unsigned int writeNum;
}ETG_FIFO;
//外部调用函数
////单字节写入
bool ETG_FIFO_WriteByte(ETG_FIFO *fifo,unsigned char data);
////获取可读长度
unsigned int ETG_FIFO_GetR_Len(ETG_FIFO *fifo);
////读取全部数据
bool ETG_FIFO_ReadAll(ETG_FIFO *fifo,unsigned char *data,bool haveRead);
////FIFO初始化
bool ETG_FIFO_Init(ETG_FIFO *fifo);
#ifdef ENALBE_ETG_FIFO_TEST
void ETG_FIFO_TEST(void);
#endif
#endif /* ETG_FIFO_h */
