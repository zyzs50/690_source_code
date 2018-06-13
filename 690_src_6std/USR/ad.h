#ifndef __AD_H__
#define __AD_H__
#include "pbdata.h"

extern u8 AD_MODE;// 0 ：单通道模式，  1：多通道模式
extern u8 G_Step_Mode_Plus;//步进脉冲
extern u16 AD_K_Wave_channel[4][8]; //存放8通道校正值

extern u16 AD_Average[12][8]; //存放平均值

extern void ADC_Configuration(u8 index);//AD1初始化
extern void ADC_Configuration_Mode1(void);//AD1初始化模式1 单通道多次
extern void ADC_Configuration_Mode2(void);//AD1初始化模式2 多通道多次
extern void DMA_Configuration(u8 mode);//DMA初始化
extern void DMA_Configuration_Mode1(u32 dst_addr);
extern void DMA_Configuration_Mode2(u32 dst_addr);

extern void Process_AD_Value(u8 mode);//数据处理
extern void Printf_AD_Value_Average(void);//输出AD测试数据的平均值
extern void Printf_AD_Value(u8 mode);//串口输出AD测试数据
extern void USB_Output_AD_Value(u8 mode);//USB端口输出AD测试数据
extern void Clear_AD_Value(u8 mode);//清除AD测试数据数组
extern u16  AD_Sample_Mode0(u8 channel_index);//单通道单次
extern void AD_Sample_Mode1(u8 channel_index);//AD单通道DMA采集
extern void AD_Sample_Mode2(u8 colum);//AD多通道DMA采集，通道数量由M决定
extern void Get_Adc_Mode0(u8 col,u8 speed);//采集第几列数据


// extern u16 Get_Adc_Average(u8 ch,u16 times);//
extern float Get_Adc_Average(u8 ch,u16 times);//
extern void  Adc_Init(void);


#endif
