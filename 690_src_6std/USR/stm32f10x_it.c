/**
  ******************************************************************************
  * @file GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and 
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"	 
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
//�����ϵͳӲ���� ��ucosͷ�ļ���ͷ�ļ����ݲ���Ҫ�Ķ�
#include "pbdata.h"

//ʹ��USBģ����Ҫ������ͷ�ļ�
#include "usb_lib.h"
#include "usb_istr.h"
// #include "hw_config.h"


//����ȫ�����Զ���ͷ�ļ����û����޸�����
#include "uart.h"
#include "motor.h"

void NMI_Handler(void)
{
}

/****************************************************************************
* ��    �ƣ�void USART1_IRQHandler(void)
* ��    �ܣ�USART1�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ���� ���� �� ���� ��ò�Ҫͬʱ���У��������׳�������
* ���÷������� 
****************************************************************************/

void USART1_IRQHandler(void)
{
	u8 receive=0;
	u8 i=0;

  if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//��ȡ�����жϱ�־λ�Ƿ�Ϊ1
  {		
		//ȷ�����ݴ�ŵ�λ��
		if(G_UART_Buffer_Size >= G_UART_BUFFERSIZE)//�������ݳ����������õĻ���
		{
			G_UART_Buffer_Size=0;
			G_UART_Input_First=0;
			//�������
			for(i=0;i<G_UART_BUFFERSIZE;i++)
			{
				G_UART_Buffer[i]=0;
			}
		}
		
		receive = USART_ReceiveData(USART1);//�Ӵ��ڽ���1�ֽڵ�����,��ȡ���־λ�Զ���0
		G_UART_Buffer[G_UART_Input_First] = receive;
		
		G_UART_Buffer_Size = G_UART_Buffer_Size+1;
		G_UART_Input_First = G_UART_Input_First+1;
		
// 		USART_SendData(USART1,receive);
// 		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
		//����յ�0XFF�������ܽ��յ��˽����ֽ�
// 		if(receive==0XFF)
// 		{
// 			G_UART_Data_Come = 1;
// 		}
// 		USART_SendData(USART1,8);
// 		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

// 		if(G_CMD_Recive_Len == 11)//�ﵽ11�ֽ�
// 		{
// 			G_CMD_Recive_Len = 0;
// 			if(receive==0XFF)
// 			{
// 				
// 				G_UART_Receive_Finish =1;//������ɱ�־
// 				USART_SendData(USART1,G_UART_Receive_Finish);
// 				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
				
// 				if(G_UART_Buffer[0]==0x0b)//���յ�ֹͣ����ָ��
// 				{
// 					plate_motor_stop_flag = 1;//���ֹͣ
// 					if(G_UART_Buffer_Size >= UART_BUFFERSIZE)//ָ��Ȳ����ܳ���50��
// 					{
// 						G_UART_Buffer_Size=0;
// 						G_UART_Input_First=0;
// 						//�������
// 						for(i=0;i<UART_BUFFERSIZE;i++)
// 						{
// 							G_UART_Buffer[i]=0;
// 						}
// 					}
// 				}
// 			}
// 			G_UART_Buffer_Size=0;
// 			G_UART_Input_First=0;
		}
  
}

/****************************************************************************
* ��    �ƣ�void USART2_IRQHandler(void)
* ��    �ܣ�USART2�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART2_IRQHandler(void)
{
// 	u8 receive=0;
// 	u8 i=0;
// 	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	

//   if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//��ȡ�����жϱ�־λ�Ƿ�Ϊ1
//   {		
// 		//ȷ�����ݴ�ŵ�λ��
// 		if(G_UART_Buffer_Size==0)
// 		{
// 			G_UART_Input_First=0;
// 		}	
// 		
// 		if(G_UART_Buffer_Size >= UART_BUFFERSIZE)//ָ��Ȳ����ܳ���50��
// 		{
// 			G_UART_Buffer_Size=0;
// 			G_UART_Input_First=0;
// 			//�������
// 			for(i=0;i<UART_BUFFERSIZE;i++)
// 			{
// 				G_UART_Buffer[i]=0;
// 			}
// 		}

// 		receive = USART_ReceiveData(USART2);//�Ӵ��ڽ���1�ֽڵ�����,��ȡ���־λ�Զ���0
// 		G_UART_Buffer[G_UART_Input_First] = receive;
// 		G_UART_Buffer_Size = G_UART_Buffer_Size+1;
// 		G_UART_Input_First = G_UART_Input_First+1;
// 		
// 		USART_SendData(USART2,receive);
// 		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);

// 		if(receive==0x1B)//
// 		{
// 			
// 		}
// 		else if(receive==0XFF)
// 		{
// 			G_UART_Receive_Finish =1;//������ɱ�־
// 			USART_SendData(USART2,G_UART_Receive_Finish);
// 		  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);

// 			G_UART_Buffer_Size=0;
// 			G_UART_Input_First=0;
// 			if(G_UART_Buffer[0]==0x0b)//���յ�ֹͣ����ָ��
// 			{
// 				plate_motor_stop_flag = 1;//���ֹͣ
// 				if(G_UART_Buffer_Size >= UART_BUFFERSIZE)//ָ��Ȳ����ܳ���50��
// 				{
// 					G_UART_Buffer_Size=0;
// 					G_UART_Input_First=0;
// 					//�������
// 					for(i=0;i<UART_BUFFERSIZE;i++)
// 					{
// 						G_UART_Buffer[i]=0;
// 					}
// 				}
// 			}
// 		}
//   }
	
	
// 	USART_ClearFlag(USART2, USART_FLAG_TC);//��־λ�����Զ���0�����ֶ�
// 	if(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=RESET)//��������жϣ�������Ϊ��
// 	{
// 		USART_SendData(USART2,'A');
// 		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
// 	}
// 	USART_ClearFlag(USART2, USART_FLAG_TXE);//��־λ�����Զ���0���ֶ�Ҳ�޷���0���´η�������ʱ�Զ���0
// 	if(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=RESET)//��������жϣ�������Ϊ��
// 	{
// 		USART_ClearFlag(USART2, USART_FLAG_TC);//��־λ�����Զ���0�����ֶ�
// 		USART_SendData(USART2,'A');
// 		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
// 		USART_SendData(USART2,'b');
// 		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
// 		USART_ClearFlag(USART2, USART_FLAG_TC);//��־λ�����Զ���0�����ֶ�
//  		  USART_ITConfig(USART2,USART_IT_TXE,DISABLE);//��ֹ���� �ķ����ж�
// 	}

}


/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval : None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval : None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval : None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval : None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval : None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval : None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval : None
  */
// void PendSV_Handler(void)
// {
// }

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_Handler(void)
{
    OSIntEnter();	//�����ж�
    OSTimeTick();       //����ucos��ʱ�ӷ������               
    OSIntExit();        //���������л����ж�
}

/****************************************************************************
* ��    �ƣ�void EXTI9_5_IRQHandler(void)
* ��    �ܣ�EXTI9-5�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line5)==SET)
  {
   	EXTI_ClearITPendingBit(EXTI_Line5);

	delay_ms(10);

	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==Bit_RESET)
	{
		if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_5)==Bit_RESET)
		{
			//LED Ϩ��
		   GPIO_SetBits(GPIOA,GPIO_Pin_5);	
		}
		else
		{
		   //LED ����
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		}
	}
  }	
}

/****************************************************************************
* ��    �ƣ�void EXTI1_IRQHandler(void)
* ��    �ܣ�EXTI2�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI1_IRQHandler(void)
{
    
}

/****************************************************************************
* ��    �ƣ�void EXTI2_IRQHandler(void)
* ��    �ܣ�EXTI2�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI2_IRQHandler(void)
{
    
}

/****************************************************************************
* ��    �ƣ�void EXTI3_IRQHandler(void)
* ��    �ܣ�EXTI3�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI3_IRQHandler(void)
{
   
}
