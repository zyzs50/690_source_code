#ifndef __USB_CONFIG_H__
#define __USB_CONFIG_H__
#include "pbdata.h"
#include "HW_Config.h"


#define USB_BUFFERSIZE 200      //usb���ڻ�������С

extern u8 G_USB_Buffer[USB_BUFFERSIZE];//USB���ڻ�����
extern u8 G_USB_Buffer_Size;//�������ڵ����ݸ���
extern u8 G_USB_Input_First;//�����������ݴ�ŵ���ʼ����
extern u8 G_USB_Buf_Full;//�������������Ƿ��Ѿ�����

extern u8 Copy_To_Usb_Buf(uint8_t*src,uint8_t len,uint8_t *dst);
extern void USB_Buffer_CMD_Clear(void);

extern void USB_Configuration(void);


#endif



