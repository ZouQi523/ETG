//
//  ETG_FIFO.c
//  ETG
//
//  Created by 邹琪 on 2019/8/23.
//  Copyright © 2019 邹琪. All rights reserved.
//

#include "ETG_FIFO.h"
void __moveRead(ETG_FIFO *fifo,unsigned int len)
{
    if((*fifo).readNum+len>(*fifo).bufferSize)
    {
        (*fifo).readNum=len-((*fifo).bufferSize-(*fifo).readNum);
        if((*fifo).readNum<=(*fifo).writeNum&&(*fifo).isFull==true)
            (*fifo).isFull=false;
    }
    else
        (*fifo).readNum+=len;
}
bool ETG_FIFO_WriteByte(ETG_FIFO *fifo,unsigned char data)
{   //单字节写入
    if((*fifo).isFull==true&&(*fifo).enalbeLoopWrite==false)
        return false;
    
    if((*fifo).writeLock == false)
        (*fifo).writeLock = true;
    else
        return false;
    
    (*fifo).buffer[(*fifo).writeNum] = data;
    (*fifo).writeNum++;
    
    if((*fifo).writeNum == (*fifo).bufferSize)
    {
        if((*fifo).enalbeLoopWrite==false)
        {   //启动非循环写入模式
            (*fifo).isFull = true;
            (*fifo).writeLock = false;
            return true;
        }
        if((*fifo).isFull == true)
            (*fifo).readNum = 0;
        (*fifo).writeNum =0;
        (*fifo).isFull = true;
    }
    if((*fifo).isFull==true&&(*fifo).writeNum>(*fifo).readNum)
        (*fifo).readNum = (*fifo).writeNum;
    
    (*fifo).writeLock = false;
    return true;
}
bool ETG_FIFO_WriteBuffer(ETG_FIFO *fifo,unsigned char *data,unsigned int dataLen)
{
    unsigned int i = 0;
    for(i=0;i<dataLen;i++)
    {
        if (ETG_FIFO_WriteByte(fifo, *(data+i))==false)
            return false;
    }
    return true;
}
unsigned int ETG_FIFO_GetR_Len(ETG_FIFO *fifo)
{   //获取可读数据的长度
    if((*fifo).writeLock == true)
        return 0;
    if((*fifo).isFull == true)
    {
        if((*fifo).writeNum==(*fifo).readNum)
            return (*fifo).bufferSize;
        return (*fifo).bufferSize-(*fifo).readNum+(*fifo).writeNum;
    }
    else
        return (*fifo).writeNum - (*fifo).readNum;
}
bool ETG_FIFO_ReadAll(ETG_FIFO *fifo,unsigned char *data,bool haveRead)
{   //功能：读取全部数据 入口参数：存储数据的地址;haveRead 是否标记为已读
    unsigned int len=0;
    if((*fifo).writeLock == true)
        return false;
    if(ETG_FIFO_GetR_Len(fifo)==0)
        return false;
    if((*fifo).isFull == true)
    {
        len=(*fifo).bufferSize-(*fifo).readNum;
        memcpy(data,&(*fifo).buffer[(*fifo).readNum],len);
        memcpy(data+len,&(*fifo).buffer[0],(*fifo).writeNum);
        if(haveRead == true)
        {
            (*fifo).readNum = (*fifo).writeNum;
            (*fifo).isFull = false;
        }
    }
    else
    {
        memcpy(data,&(*fifo).buffer[(*fifo).readNum],(*fifo).writeNum-(*fifo).readNum);
        if(haveRead == true)
             (*fifo).readNum = (*fifo).writeNum;
    }
    return true;
}
#ifdef ENABLE_ETG_FIFO_STR

#endif
bool ETG_FIFO_Init(ETG_FIFO *fifo)
{
    (*fifo).isFull = false;
    (*fifo).writeLock = false;
    (*fifo).readNum = 0;
    (*fifo).writeNum = 0;
    memset((*fifo).buffer,0,(*fifo).bufferSize);
    return true;
}
#ifdef ENALBE_ETG_FIFO_TEST
void __FIFO_InfoPrint(ETG_FIFO *fifo)
{   //用于显示fifo的状态信息
    printf("w:%d,r:%d,l:%d,isFull:%d,lock:%d\r\n",(*fifo).writeNum,(*fifo).readNum,ETG_FIFO_GetR_Len(fifo),(*fifo).isFull,(*fifo).writeLock);
}
void __FIFO_BufferPrint(ETG_FIFO *fifo)
{
    unsigned int i=0;
    printf("FIFO Data:");
    for(i=0;i<(*fifo).bufferSize;i++)
    {
        printf("%2X",*((*fifo).buffer+i));
    }
    printf("\r\n");
}
void ETG_FIFO_TEST(void)
{
#define testBufferSize 10
    unsigned int i=0;
    unsigned int testNum=1;
    //配置信息
    unsigned char buffer[testBufferSize];
    unsigned char testbuffer[testBufferSize*2];
    unsigned int buffer_size = testBufferSize;
    ETG_FIFO fifo;
    //初始化fifo
    fifo.enalbeLoopWrite = true;
    fifo.buffer=buffer;
    fifo.bufferSize=buffer_size;
    ETG_FIFO_Init(&fifo);
    //test1测试
    printf("TEST %d 单次满buffer写入\r\n",testNum);
    for(i=0;i<buffer_size;i++)
        ETG_FIFO_WriteByte(&fifo, i%255);
    __FIFO_InfoPrint(&fifo);
    __FIFO_BufferPrint(&fifo);
    testNum++;
    // 单次不满写入
    ETG_FIFO_Init(&fifo);//清零写入数据
    printf("TEST %d 单次不满写入\r\n",testNum);
    for(i=0;i<buffer_size/2;i++)
        ETG_FIFO_WriteByte(&fifo, i%255);
    __FIFO_InfoPrint(&fifo);
    __FIFO_BufferPrint(&fifo);
    testNum++;
    //测试循环写入
    ETG_FIFO_Init(&fifo);//清零写入数据
    printf("TEST %d 测试循环写入\r\n",testNum);
    for(i=0;i<buffer_size+2;i++)
        ETG_FIFO_WriteByte(&fifo, i%255);
    __FIFO_BufferPrint(&fifo);
    __FIFO_InfoPrint(&fifo);
    testNum++;
    //批量写入测试
    ETG_FIFO_Init(&fifo);//清零写入数据
    printf("TEST %d 批量写入测试\r\n",testNum);
    for(i=0;i<buffer_size;i++)
        testbuffer[i]=i%255;
    ETG_FIFO_WriteBuffer(&fifo, testbuffer, buffer_size);
    __FIFO_BufferPrint(&fifo);
    __FIFO_InfoPrint(&fifo);
    testNum++;
    //批量写入循环测试
    ETG_FIFO_Init(&fifo);//清零写入数据
    printf("TEST %d 批量写入循环测试\r\n",testNum);
    for(i=0;i<buffer_size+2;i++)
        testbuffer[i]=i%255;
    ETG_FIFO_WriteBuffer(&fifo, testbuffer, buffer_size+2);
    ETG_FIFO_WriteBuffer(&fifo, testbuffer, buffer_size);
    __FIFO_BufferPrint(&fifo);
    __FIFO_InfoPrint(&fifo);
    testNum++;
    //读取不满的FIFO数据
    ETG_FIFO_Init(&fifo);//清零写入数据
    printf("TEST %d 读取不满的FIFO数据\r\n",testNum);
    for(i=0;i<buffer_size-2;i++)
        ETG_FIFO_WriteByte(&fifo, i%255);
    __FIFO_BufferPrint(&fifo);
    __FIFO_InfoPrint(&fifo);
    printf("不标记已读:");
    ETG_FIFO_ReadAll(&fifo, testbuffer, false);//不标记已读
    __FIFO_InfoPrint(&fifo);
    printf("标记已读:");
    ETG_FIFO_ReadAll(&fifo, testbuffer, true);//不标记已读
    __FIFO_InfoPrint(&fifo);
    testNum++;
    //读取满的FIFO数据
    ETG_FIFO_Init(&fifo);//清零写入数据
    printf("TEST %d 读取满的FIFO数据\r\n",testNum);
    for(i=0;i<buffer_size+5;i++)
        ETG_FIFO_WriteByte(&fifo, i%255);
    __FIFO_BufferPrint(&fifo);
    __FIFO_InfoPrint(&fifo);
    printf("不标记已读:");
    ETG_FIFO_ReadAll(&fifo, testbuffer, false);//不标记已读
    __FIFO_InfoPrint(&fifo);
    printf("标记已读:");
    ETG_FIFO_ReadAll(&fifo, testbuffer, true);//不标记已读
    __FIFO_InfoPrint(&fifo);
    testNum++;
}
#endif
