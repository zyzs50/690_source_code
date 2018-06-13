#ifndef __UART_H__
#define __UART_H__
#include "pbdata.h"

//---------����-------------------------
#define G_UART_BUFFERSIZE 50      //���ڻ�������С
extern u8 G_UART_Buffer[G_UART_BUFFERSIZE];//���ڻ�����
extern u8 G_UART_Buffer_Size;//�������ڵ����ݸ���
extern u8 G_UART_Input_First;//�����������ݴ�ŵ���ʼ����
extern u8 G_UART_Data_Come;//��������1�յ�����


extern void USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity);//���ڳ�ʼ��
extern u8 UART_Receive_Data_Over(void);//�жϴ����Ƿ���յ���Чָ��
extern void UART_Buffer_CMD_Send(void);//�Ӵ���ָ���������ָ��
extern void UART_Buffer_CMD_Send1(u8 *datacmd ,u8 len);//�Ӵ���ָ���������ָ��
extern void UART_Buffer_CMD_Clear(void);//�������ָ�����
extern u8 UART_Receive_Data_Over(void);//�ȴ��������ݽ������
extern u8 UART_Receive_Data_Valid(void);//�жϴ������ݽ��յ�ָ���Ƿ���Ч



#endif

