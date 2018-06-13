/****************************************************************************
* 文件名称：uart.c
* 说    明：串口文件
* 功    能：里面主要放串口涉及到的动作函数
* 内置方法1：void USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity)
* 内置方法2：int fputc(int ch,FILE *f)        标准库函数重载，这样就能使用printf函数了
* 内置方法3：
* 内置方法4：
* 内置方法5：
* 内置方法6：
* 内置方法7：
* 内置方法8：
* 内置方法9：
* 内置方法10：
* 内置方法11：
****************************************************************************/
#include "uart.h"
#include "crc32.h"

//-----------------------------------串口涉及到的变量---------------------------------------------

u8 G_UART_Buffer[G_UART_BUFFERSIZE]={0};//串口缓冲区
u8 G_UART_Buffer_Size = 0;//缓冲区内的数据个数
u8 G_UART_Input_First = 0;//缓存区内数据存放的起始索引
u8 G_UART_Data_Come = 0;//表明串口1收到数据



/****************************************************************************
* 名    称：USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity)
* 功    能：串口初始化
* 入口参数1：串口号
* 入口参数2：波特率
* 入口参数3：数据位
* 入口参数4：停止位
* 入口参数5：校验位
* 出口参数：无
* 说    明：无
* 调用方法：无 
****************************************************************************/
void USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity)
{
	USART_InitTypeDef  USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate=BaudRate;//波特率:9600
	USART_InitStructure.USART_WordLength=WordLength;//数据位: 8位
	USART_InitStructure.USART_StopBits=StopBits;//停止位: 1位
	USART_InitStructure.USART_Parity=Parity;//校验位: 无
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//硬件流控: 无
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//工作模式：发送使能，接收使能

	USART_Init(USARTx,&USART_InitStructure);//串口1初始化
	USART_ITConfig(USARTx,USART_IT_RXNE,ENABLE);//使能串口 的接收中断
//   USART_ITConfig(USARTx,USART_IT_TXE,ENABLE);//使能串口 的发送中断
 	USART_Cmd(USARTx,ENABLE);//打开使能串口1  此时串口1就可以正常工作了
	USART_ClearFlag(USARTx,USART_FLAG_TC);//清除串口1 的发送完成中断标志位
	
}



/****************************************************************************
* 名    称：u8 UART_Receive_Data_Over(void)
* 功    能：判断串口是否接收完了指令
* 入口参数：无
* 出口参数：0：接收到的指令正确有效，   1：指令无效
* 说    明：
* 调用方法：
****************************************************************************/
u8 UART_Receive_Data_Over(void)
{
	
	//1.检测串口是否收到数据
	if (G_UART_Data_Come)//表明串口接收到数据
	{
		G_UART_Data_Come = 0;
		delay_ms(10);//延时等待串口接收完数据 ，一条指令11个字节，20ms足够接收完
		//2.再次检测串口是否收到数据
		if(G_UART_Data_Come) 
		{
			G_UART_Data_Come = 0;
		}
		return 0;//数据接收完毕
	}
	else
	{
		return 1;//串口无数据
	}
}

/****************************************************************************
* 名    称：u8 UART_Receive_Data_Over(void)
* 功    能：判断串口是否接收到了正确的指令
* 入口参数：无
* 出口参数：0：接收到的指令正确有效，   1：指令无效
* 说    明：
* 调用方法：
****************************************************************************/
u8 UART_Receive_Data_Valid(void)
{
	u32 crc_result =0;
	
	//判断缓冲区数据是否接收完整，一条指令11个字节
	
	if(G_UART_Buffer_Size >=11)// 接收完成
	{
		return 0;
		
		//判断指令是否有效
		crc_result = make_crc(G_UART_Buffer,6);//生成CRC32校验码
		printf("crc_result = %d\r\n",crc_result);
		printf("%d = %d\r\n",G_UART_Buffer[6],crc_result>>24);
		printf("%d = %d\r\n",G_UART_Buffer[7],(crc_result&0x00ff0000)>>16);
		printf("%d = %d\r\n",G_UART_Buffer[8],(crc_result&0x0000ff00)>>8);
		printf("%d = %d\r\n",G_UART_Buffer[9],(crc_result&0x000000ff));
		
		if((G_UART_Buffer[6]==crc_result>>24)&&(G_UART_Buffer[7]==(crc_result&0x00ff0000)>>16)&&(G_UART_Buffer[8]==(crc_result&0x0000ff00)>>8)&&(G_UART_Buffer[9]==(crc_result&0x000000ff)))
		{
			return 0;//指令有效
		}
		else
		{
			UART_Buffer_CMD_Clear();//清除串口缓冲区
			return 1;//指令无效
		}
	}
	else//收到不足11字节的指令
	{
		return 1;//指令无效
	}
}


/****************************************************************************
* 名    称：void UART_Buffer_CMD_Clear(void)
* 功    能：清除串口指令缓冲区
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void UART_Buffer_CMD_Clear(void)
{
	u8 i=0;
	
	G_UART_Buffer_Size=0;
	G_UART_Input_First=0;
	//清除缓存
	for(i=0;i<G_UART_BUFFERSIZE;i++)
	{
		G_UART_Buffer[i]=0;
	}
}

/****************************************************************************
* 名    称：void UART_Buffer_CMD_Send(void)
* 功    能：发送串口指令缓冲区
* 入口参数：无
* 出口参数：无
* 说    明：通讯指令 设置的 11 字节长度
* 调用方法：
****************************************************************************/
void UART_Buffer_CMD_Send(void)
{
	u8 send_data=0;
	u8 send_count=0;

	while(send_count < 11)
	{
		send_data = G_UART_Buffer[send_count];
		USART_SendData(USART1,send_data);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		send_count++;
	}
}

/****************************************************************************
* 名    称：void UART_Buffer_CMD_Send1(u8 *datacmd ,u8 len)
* 功    能：发送串口指令缓冲区1
* 入口参数：无
* 出口参数：无
* 说    明：通讯指令 设置的 2 字节长度
* 调用方法：
****************************************************************************/
void UART_Buffer_CMD_Send1(u8 *datacmd ,u8 len)
{
	u8 send_data=0;
	u8 send_count=0;

	while(send_count < len)
	{
		send_data = datacmd[send_count];
		USART_SendData(USART1,send_data);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		send_count++;
	}
}


