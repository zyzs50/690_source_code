#include "usb_config.h"


u8 G_USB_Buffer[USB_BUFFERSIZE]={0};//USB串口缓冲区
u8 G_USB_Buffer_Size = 0;//缓冲区内的数据个数
u8 G_USB_Input_First = 0;//缓存区内数据存放的起始索引
u8 G_USB_Buf_Full = 0;//缓存区内数据是否已经存满


/****************************************************************************
* 名    称：u8 Copy_To_Usb_Buf(uint8_t*src,uint8_t len,uint8_t *dst)
* 功    能：接收到的数据复制到USB缓存
* 入口参数：
* 出口参数：返回是否成功标志 0：成功 1：失败
* 说    明：USB缓存 存满后就不在保存数据，这有可能造成后续数据的丢失， 但是USB缓存用来存放上位机发送的指令，数据量不大，
* 调用方法：
****************************************************************************/
u8 Copy_To_Usb_Buf(uint8_t*src,uint8_t len,uint8_t *dst)
{
	uint8_t i=0;
	uint8_t count=len;
	
	if ((G_USB_Buffer_Size + len)>=USB_BUFFERSIZE)
	{
		count = USB_BUFFERSIZE - G_USB_Buffer_Size;
		G_USB_Buf_Full = 1;
	}
	
	for(i=0;i<count;i++)
	{
		dst[G_USB_Input_First] = src[i];
		G_USB_Input_First += 1;
		G_USB_Buffer_Size += 1;
	}
	return 0;
}


/****************************************************************************
* 名    称：void UART_Buffer_CMD_Clear(void)
* 功    能：清除串口指令缓冲区
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void USB_Buffer_CMD_Clear(void)
{
	u8 i=0;
	
	G_USB_Buffer_Size=0;
	G_USB_Input_First=0;
	//清除缓存
	for(i=0;i<USB_BUFFERSIZE;i++)
	{
		G_USB_Buffer[i]=0;
	}
	G_USB_Buf_Full = 0;
}

/****************************************************************************
* 名    称：void USB_Configuration(void)
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void USB_Configuration(void)
{
    USB_Config();
}


