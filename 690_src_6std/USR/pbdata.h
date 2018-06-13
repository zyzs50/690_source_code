#ifndef __pbdata_H__
#define __pbdata_H__

//通用固件库头文件
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_flash.h"
#include "stdio.h"

//ucosii头文件
#include "includes.h"


//定义指令功能标志
#define  FUNC_TYPE_CONNECT 						0x03
#define  FUNC_TYPE_SELF_TEST 					0x04
#define  FUNC_TYPE_PREPARE_TEST 			0x06
#define  FUNC_TYPE_START_TEST 				0x07
#define  FUNC_TYPE_STOP_TEST 					0x08
#define  FUNC_TYPE_COM_OPEN 					0x09
#define  FUNC_TYPE_COM_CLOSE 					0x0A
#define  FUNC_TYPE_USB_OPEN 					0x0B
#define  FUNC_TYPE_USB_CLOSE 					0x0C
#define  FUNC_TYPE_LIGHT_ON 					0x0D
#define  FUNC_TYPE_LIGHT_OFF 					0x0E
#define  FUNC_TYPE_TEMPERATURE_READ 	0x12
#define  FUNC_TYPE_TEMPERATURE_ON 		0x13
#define  FUNC_TYPE_TEMPERATURE_OFF 		0x14
#define  FUNC_TYPE_TEMPERATURE_HOT 		0x15
#define  FUNC_TYPE_TEMPERATURE_COLD 	0x16
#define  FUNC_TYPE_PLATE_RESET 				0x17
#define  FUNC_TYPE_PLATE_RUN 					0x18
#define  FUNC_TYPE_PLATE_POSITION 		0x19
#define  FUNC_TYPE_PLATE_SHAKE 				0x1A
#define  FUNC_TYPE_ABS_TEST 					0x1B
#define  FUNC_TYPE_ABS_STOP 					0x1C
#define  FUNC_TYPE_LUMINOUS_TEST 			0x1D
#define  FUNC_TYPE_LUMINOUS_STOP 			0x1E
#define  FUNC_TYPE_FILTER_RESET 			0x23
#define  FUNC_TYPE_FILTER_POSITION 		0x24





extern u8  fac_us;//us延时倍乘数
extern u16 fac_ms;//ms延时倍乘数

//自定义的全局变量声明
extern u8 G_Curr_Cmd_Type;//当前命令类型
extern u8 G_Pre_Cmd_Type; //之前命令类型
extern u8 G_Error_Type;
extern u8 G_Computer_Connect_Flag; //上位机联机成功标志 0:上位机未连接， 1：上位机已经连接

extern OS_EVENT *Func_Sem; //ucosii 信号量
extern u8 G_Func_Running;//表示FUNC任务正在执行某命令动作
extern u8 G_Func_Type;//指示哪种任务
extern u8 G_Temperature_Disp;//指示是否显示温度
extern u8 G_Heat1_Start;//指示是否开始加热
extern u8 G_Heat2_Start;//指示是否开始加热
extern u8 G_Heat3_Start;//指示是否开始加热
extern u16 Target_Temperature;

extern u8 G_Power_On_Off;//指示是否开启灯源
extern u8 G_Power_On_Off_Fre;//灯源频率

extern u8 G_Beep_on_off; //蜂鸣器开关
extern u8 G_Beep_count; //蜂鸣器频率

extern u8 DS18B20_ID01[8];//DS18B20温度传感器1 ID
extern u8 DS18B20_ID02[8];//DS18B20温度传感器2 ID
extern u8 DS18B20_ID03[8];//DS18B20温度传感器3 ID
extern u8 DS18B20_ID04[8];//DS18B20温度传感器4 ID

//自定义函数声明
extern void RCC_HSE_Configuration(void); //HSE作为PLL时钟，PLL作为SYSCLK 
extern void RCC_Configuration(void);//时钟初始化
extern void GPIO_Configuration(void);//GPIO初始化
extern void SysTick_Configuration(void);//SysTick初始化
extern void EXIT_Configuraation(void);//外部中断初始化
extern void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority); //中断优先级初始化

//ucosii相关
extern void Delay_Init(void);	//系统滴答时钟初始化
extern void delay_us(u32 nus);//ucosii下微妙级延时
extern void delay_ms(u16 nms);//ucosii下毫秒级延时


#endif
