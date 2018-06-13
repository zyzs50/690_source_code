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
//里面放系统硬件库 或ucos头文件，头文件内容不需要改动
#include "pbdata.h"

//使用USB模块需要包含的头文件
#include "usb_lib.h"
#include "usb_istr.h"
// #include "hw_config.h"


//下面全部是自定义头文件，用户可修改内容
#include "uart.h"
#include "motor.h"

void NMI_Handler(void)
{
}

/****************************************************************************
* 名    称：void USART1_IRQHandler(void)
* 功    能：USART1中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明： 发送 和 接收 最好不要同时进行，否则容易出现死机
* 调用方法：无 
****************************************************************************/

void USART1_IRQHandler(void)
{
	u8 receive=0;
	u8 i=0;

  if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//读取接收中断标志位是否为1
  {		
		//确定数据存放的位置
		if(G_UART_Buffer_Size >= G_UART_BUFFERSIZE)//接收数据超过串口设置的缓存
		{
			G_UART_Buffer_Size=0;
			G_UART_Input_First=0;
			//清除缓存
			for(i=0;i<G_UART_BUFFERSIZE;i++)
			{
				G_UART_Buffer[i]=0;
			}
		}
		
		receive = USART_ReceiveData(USART1);//从串口接收1字节的数据,读取后标志位自动清0
		G_UART_Buffer[G_UART_Input_First] = receive;
		
		G_UART_Buffer_Size = G_UART_Buffer_Size+1;
		G_UART_Input_First = G_UART_Input_First+1;
		
// 		USART_SendData(USART1,receive);
// 		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
		//如果收到0XFF表明可能接收到了结束字节
// 		if(receive==0XFF)
// 		{
// 			G_UART_Data_Come = 1;
// 		}
// 		USART_SendData(USART1,8);
// 		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

// 		if(G_CMD_Recive_Len == 11)//达到11字节
// 		{
// 			G_CMD_Recive_Len = 0;
// 			if(receive==0XFF)
// 			{
// 				
// 				G_UART_Receive_Finish =1;//接收完成标志
// 				USART_SendData(USART1,G_UART_Receive_Finish);
// 				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
				
// 				if(G_UART_Buffer[0]==0x0b)//接收到停止测试指令
// 				{
// 					plate_motor_stop_flag = 1;//电机停止
// 					if(G_UART_Buffer_Size >= UART_BUFFERSIZE)//指令长度不可能超过50个
// 					{
// 						G_UART_Buffer_Size=0;
// 						G_UART_Input_First=0;
// 						//清除缓存
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
* 名    称：void USART2_IRQHandler(void)
* 功    能：USART2中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART2_IRQHandler(void)
{
// 	u8 receive=0;
// 	u8 i=0;
// 	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	

//   if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//读取接收中断标志位是否为1
//   {		
// 		//确定数据存放的位置
// 		if(G_UART_Buffer_Size==0)
// 		{
// 			G_UART_Input_First=0;
// 		}	
// 		
// 		if(G_UART_Buffer_Size >= UART_BUFFERSIZE)//指令长度不可能超过50个
// 		{
// 			G_UART_Buffer_Size=0;
// 			G_UART_Input_First=0;
// 			//清除缓存
// 			for(i=0;i<UART_BUFFERSIZE;i++)
// 			{
// 				G_UART_Buffer[i]=0;
// 			}
// 		}

// 		receive = USART_ReceiveData(USART2);//从串口接收1字节的数据,读取后标志位自动清0
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
// 			G_UART_Receive_Finish =1;//接收完成标志
// 			USART_SendData(USART2,G_UART_Receive_Finish);
// 		  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);

// 			G_UART_Buffer_Size=0;
// 			G_UART_Input_First=0;
// 			if(G_UART_Buffer[0]==0x0b)//接收到停止测试指令
// 			{
// 				plate_motor_stop_flag = 1;//电机停止
// 				if(G_UART_Buffer_Size >= UART_BUFFERSIZE)//指令长度不可能超过50个
// 				{
// 					G_UART_Buffer_Size=0;
// 					G_UART_Input_First=0;
// 					//清除缓存
// 					for(i=0;i<UART_BUFFERSIZE;i++)
// 					{
// 						G_UART_Buffer[i]=0;
// 					}
// 				}
// 			}
// 		}
//   }
	
	
// 	USART_ClearFlag(USART2, USART_FLAG_TC);//标志位不会自动清0，需手动
// 	if(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=RESET)//发送完成中断，缓冲区为空
// 	{
// 		USART_SendData(USART2,'A');
// 		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
// 	}
// 	USART_ClearFlag(USART2, USART_FLAG_TXE);//标志位不会自动清0，手动也无法清0，下次发送数据时自动清0
// 	if(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=RESET)//发送完成中断，缓冲区为空
// 	{
// 		USART_ClearFlag(USART2, USART_FLAG_TC);//标志位不会自动清0，需手动
// 		USART_SendData(USART2,'A');
// 		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
// 		USART_SendData(USART2,'b');
// 		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
// 		USART_ClearFlag(USART2, USART_FLAG_TC);//标志位不会自动清0，需手动
//  		  USART_ITConfig(USART2,USART_IT_TXE,DISABLE);//禁止串口 的发送中断
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
    OSIntEnter();	//进入中断
    OSTimeTick();       //调用ucos的时钟服务程序               
    OSIntExit();        //触发任务切换软中断
}

/****************************************************************************
* 名    称：void EXTI9_5_IRQHandler(void)
* 功    能：EXTI9-5中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
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
			//LED 熄灭
		   GPIO_SetBits(GPIOA,GPIO_Pin_5);	
		}
		else
		{
		   //LED 发光
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		}
	}
  }	
}

/****************************************************************************
* 名    称：void EXTI1_IRQHandler(void)
* 功    能：EXTI2中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI1_IRQHandler(void)
{
    
}

/****************************************************************************
* 名    称：void EXTI2_IRQHandler(void)
* 功    能：EXTI2中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI2_IRQHandler(void)
{
    
}

/****************************************************************************
* 名    称：void EXTI3_IRQHandler(void)
* 功    能：EXTI3中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI3_IRQHandler(void)
{
   
}
