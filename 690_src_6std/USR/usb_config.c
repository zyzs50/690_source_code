#include "usb_config.h"


u8 G_USB_Buffer[USB_BUFFERSIZE]={0};//USB���ڻ�����
u8 G_USB_Buffer_Size = 0;//�������ڵ����ݸ���
u8 G_USB_Input_First = 0;//�����������ݴ�ŵ���ʼ����
u8 G_USB_Buf_Full = 0;//�������������Ƿ��Ѿ�����


/****************************************************************************
* ��    �ƣ�u8 Copy_To_Usb_Buf(uint8_t*src,uint8_t len,uint8_t *dst)
* ��    �ܣ����յ������ݸ��Ƶ�USB����
* ��ڲ�����
* ���ڲ����������Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����USB���� ������Ͳ��ڱ������ݣ����п�����ɺ������ݵĶ�ʧ�� ����USB�������������λ�����͵�ָ�����������
* ���÷�����
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
* ��    �ƣ�void UART_Buffer_CMD_Clear(void)
* ��    �ܣ��������ָ�����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void USB_Buffer_CMD_Clear(void)
{
	u8 i=0;
	
	G_USB_Buffer_Size=0;
	G_USB_Input_First=0;
	//�������
	for(i=0;i<USB_BUFFERSIZE;i++)
	{
		G_USB_Buffer[i]=0;
	}
	G_USB_Buf_Full = 0;
}

/****************************************************************************
* ��    �ƣ�void USB_Configuration(void)
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void USB_Configuration(void)
{
    USB_Config();
}


