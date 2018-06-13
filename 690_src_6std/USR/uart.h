#ifndef __UART_H__
#define __UART_H__
#include "pbdata.h"

//---------串口-------------------------
#define G_UART_BUFFERSIZE 50      //串口缓存区大小
extern u8 G_UART_Buffer[G_UART_BUFFERSIZE];//串口缓冲区
extern u8 G_UART_Buffer_Size;//缓冲区内的数据个数
extern u8 G_UART_Input_First;//缓存区内数据存放的起始索引
extern u8 G_UART_Data_Come;//表明串口1收到数据


extern void USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity);//串口初始化
extern u8 UART_Receive_Data_Over(void);//判断串口是否接收到有效指令
extern void UART_Buffer_CMD_Send(void);//从串口指令缓冲区发送指令
extern void UART_Buffer_CMD_Send1(u8 *datacmd ,u8 len);//从串口指令缓冲区发送指令
extern void UART_Buffer_CMD_Clear(void);//清除串口指令缓冲区
extern u8 UART_Receive_Data_Over(void);//等待串口数据接收完毕
extern u8 UART_Receive_Data_Valid(void);//判断串口数据接收的指令是否有效



#endif

