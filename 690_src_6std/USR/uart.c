/****************************************************************************
* �ļ����ƣ�uart.c
* ˵    ���������ļ�
* ��    �ܣ�������Ҫ�Ŵ����漰���Ķ�������
* ���÷���1��void USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity)
* ���÷���2��int fputc(int ch,FILE *f)        ��׼�⺯�����أ���������ʹ��printf������
* ���÷���3��
* ���÷���4��
* ���÷���5��
* ���÷���6��
* ���÷���7��
* ���÷���8��
* ���÷���9��
* ���÷���10��
* ���÷���11��
****************************************************************************/
#include "uart.h"
#include "crc32.h"

//-----------------------------------�����漰���ı���---------------------------------------------

u8 G_UART_Buffer[G_UART_BUFFERSIZE]={0};//���ڻ�����
u8 G_UART_Buffer_Size = 0;//�������ڵ����ݸ���
u8 G_UART_Input_First = 0;//�����������ݴ�ŵ���ʼ����
u8 G_UART_Data_Come = 0;//��������1�յ�����



/****************************************************************************
* ��    �ƣ�USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity)
* ��    �ܣ����ڳ�ʼ��
* ��ڲ���1�����ں�
* ��ڲ���2��������
* ��ڲ���3������λ
* ��ڲ���4��ֹͣλ
* ��ڲ���5��У��λ
* ���ڲ�������
* ˵    ������
* ���÷������� 
****************************************************************************/
void USART_Configuration(USART_TypeDef* USARTx,uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity)
{
	USART_InitTypeDef  USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate=BaudRate;//������:9600
	USART_InitStructure.USART_WordLength=WordLength;//����λ: 8λ
	USART_InitStructure.USART_StopBits=StopBits;//ֹͣλ: 1λ
	USART_InitStructure.USART_Parity=Parity;//У��λ: ��
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//Ӳ������: ��
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//����ģʽ������ʹ�ܣ�����ʹ��

	USART_Init(USARTx,&USART_InitStructure);//����1��ʼ��
	USART_ITConfig(USARTx,USART_IT_RXNE,ENABLE);//ʹ�ܴ��� �Ľ����ж�
//   USART_ITConfig(USARTx,USART_IT_TXE,ENABLE);//ʹ�ܴ��� �ķ����ж�
 	USART_Cmd(USARTx,ENABLE);//��ʹ�ܴ���1  ��ʱ����1�Ϳ�������������
	USART_ClearFlag(USARTx,USART_FLAG_TC);//�������1 �ķ�������жϱ�־λ
	
}



/****************************************************************************
* ��    �ƣ�u8 UART_Receive_Data_Over(void)
* ��    �ܣ��жϴ����Ƿ��������ָ��
* ��ڲ�������
* ���ڲ�����0�����յ���ָ����ȷ��Ч��   1��ָ����Ч
* ˵    ����
* ���÷�����
****************************************************************************/
u8 UART_Receive_Data_Over(void)
{
	
	//1.��⴮���Ƿ��յ�����
	if (G_UART_Data_Come)//�������ڽ��յ�����
	{
		G_UART_Data_Come = 0;
		delay_ms(10);//��ʱ�ȴ����ڽ��������� ��һ��ָ��11���ֽڣ�20ms�㹻������
		//2.�ٴμ�⴮���Ƿ��յ�����
		if(G_UART_Data_Come) 
		{
			G_UART_Data_Come = 0;
		}
		return 0;//���ݽ������
	}
	else
	{
		return 1;//����������
	}
}

/****************************************************************************
* ��    �ƣ�u8 UART_Receive_Data_Over(void)
* ��    �ܣ��жϴ����Ƿ���յ�����ȷ��ָ��
* ��ڲ�������
* ���ڲ�����0�����յ���ָ����ȷ��Ч��   1��ָ����Ч
* ˵    ����
* ���÷�����
****************************************************************************/
u8 UART_Receive_Data_Valid(void)
{
	u32 crc_result =0;
	
	//�жϻ����������Ƿ����������һ��ָ��11���ֽ�
	
	if(G_UART_Buffer_Size >=11)// �������
	{
		return 0;
		
		//�ж�ָ���Ƿ���Ч
		crc_result = make_crc(G_UART_Buffer,6);//����CRC32У����
		printf("crc_result = %d\r\n",crc_result);
		printf("%d = %d\r\n",G_UART_Buffer[6],crc_result>>24);
		printf("%d = %d\r\n",G_UART_Buffer[7],(crc_result&0x00ff0000)>>16);
		printf("%d = %d\r\n",G_UART_Buffer[8],(crc_result&0x0000ff00)>>8);
		printf("%d = %d\r\n",G_UART_Buffer[9],(crc_result&0x000000ff));
		
		if((G_UART_Buffer[6]==crc_result>>24)&&(G_UART_Buffer[7]==(crc_result&0x00ff0000)>>16)&&(G_UART_Buffer[8]==(crc_result&0x0000ff00)>>8)&&(G_UART_Buffer[9]==(crc_result&0x000000ff)))
		{
			return 0;//ָ����Ч
		}
		else
		{
			UART_Buffer_CMD_Clear();//������ڻ�����
			return 1;//ָ����Ч
		}
	}
	else//�յ�����11�ֽڵ�ָ��
	{
		return 1;//ָ����Ч
	}
}


/****************************************************************************
* ��    �ƣ�void UART_Buffer_CMD_Clear(void)
* ��    �ܣ��������ָ�����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void UART_Buffer_CMD_Clear(void)
{
	u8 i=0;
	
	G_UART_Buffer_Size=0;
	G_UART_Input_First=0;
	//�������
	for(i=0;i<G_UART_BUFFERSIZE;i++)
	{
		G_UART_Buffer[i]=0;
	}
}

/****************************************************************************
* ��    �ƣ�void UART_Buffer_CMD_Send(void)
* ��    �ܣ����ʹ���ָ�����
* ��ڲ�������
* ���ڲ�������
* ˵    ����ͨѶָ�� ���õ� 11 �ֽڳ���
* ���÷�����
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
* ��    �ƣ�void UART_Buffer_CMD_Send1(u8 *datacmd ,u8 len)
* ��    �ܣ����ʹ���ָ�����1
* ��ڲ�������
* ���ڲ�������
* ˵    ����ͨѶָ�� ���õ� 2 �ֽڳ���
* ���÷�����
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


