#ifndef __USB_CONFIG_H__
#define __USB_CONFIG_H__
#include "pbdata.h"
#include "HW_Config.h"


#define USB_BUFFERSIZE 200      //usb串口缓存区大小

extern u8 G_USB_Buffer[USB_BUFFERSIZE];//USB串口缓冲区
extern u8 G_USB_Buffer_Size;//缓冲区内的数据个数
extern u8 G_USB_Input_First;//缓存区内数据存放的起始索引
extern u8 G_USB_Buf_Full;//缓存区内数据是否已经存满

extern u8 Copy_To_Usb_Buf(uint8_t*src,uint8_t len,uint8_t *dst);
extern void USB_Buffer_CMD_Clear(void);

extern void USB_Configuration(void);


#endif



