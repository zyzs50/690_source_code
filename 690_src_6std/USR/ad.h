#ifndef __AD_H__
#define __AD_H__
#include "pbdata.h"

extern u8 AD_MODE;// 0 ����ͨ��ģʽ��  1����ͨ��ģʽ
extern u8 G_Step_Mode_Plus;//��������
extern u16 AD_K_Wave_channel[4][8]; //���8ͨ��У��ֵ

extern u16 AD_Average[12][8]; //���ƽ��ֵ

extern void ADC_Configuration(u8 index);//AD1��ʼ��
extern void ADC_Configuration_Mode1(void);//AD1��ʼ��ģʽ1 ��ͨ�����
extern void ADC_Configuration_Mode2(void);//AD1��ʼ��ģʽ2 ��ͨ�����
extern void DMA_Configuration(u8 mode);//DMA��ʼ��
extern void DMA_Configuration_Mode1(u32 dst_addr);
extern void DMA_Configuration_Mode2(u32 dst_addr);

extern void Process_AD_Value(u8 mode);//���ݴ���
extern void Printf_AD_Value_Average(void);//���AD�������ݵ�ƽ��ֵ
extern void Printf_AD_Value(u8 mode);//�������AD��������
extern void USB_Output_AD_Value(u8 mode);//USB�˿����AD��������
extern void Clear_AD_Value(u8 mode);//���AD������������
extern u16  AD_Sample_Mode0(u8 channel_index);//��ͨ������
extern void AD_Sample_Mode1(u8 channel_index);//AD��ͨ��DMA�ɼ�
extern void AD_Sample_Mode2(u8 colum);//AD��ͨ��DMA�ɼ���ͨ��������M����
extern void Get_Adc_Mode0(u8 col,u8 speed);//�ɼ��ڼ�������


// extern u16 Get_Adc_Average(u8 ch,u16 times);//
extern float Get_Adc_Average(u8 ch,u16 times);//
extern void  Adc_Init(void);


#endif
