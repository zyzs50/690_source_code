#ifndef __pbdata_H__
#define __pbdata_H__

//ͨ�ù̼���ͷ�ļ�
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

//ucosiiͷ�ļ�
#include "includes.h"


//����ָ��ܱ�־
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





extern u8  fac_us;//us��ʱ������
extern u16 fac_ms;//ms��ʱ������

//�Զ����ȫ�ֱ�������
extern u8 G_Curr_Cmd_Type;//��ǰ��������
extern u8 G_Pre_Cmd_Type; //֮ǰ��������
extern u8 G_Error_Type;
extern u8 G_Computer_Connect_Flag; //��λ�������ɹ���־ 0:��λ��δ���ӣ� 1����λ���Ѿ�����

extern OS_EVENT *Func_Sem; //ucosii �ź���
extern u8 G_Func_Running;//��ʾFUNC��������ִ��ĳ�����
extern u8 G_Func_Type;//ָʾ��������
extern u8 G_Temperature_Disp;//ָʾ�Ƿ���ʾ�¶�
extern u8 G_Heat1_Start;//ָʾ�Ƿ�ʼ����
extern u8 G_Heat2_Start;//ָʾ�Ƿ�ʼ����
extern u8 G_Heat3_Start;//ָʾ�Ƿ�ʼ����
extern u16 Target_Temperature;

extern u8 G_Power_On_Off;//ָʾ�Ƿ�����Դ
extern u8 G_Power_On_Off_Fre;//��ԴƵ��

extern u8 G_Beep_on_off; //����������
extern u8 G_Beep_count; //������Ƶ��

extern u8 DS18B20_ID01[8];//DS18B20�¶ȴ�����1 ID
extern u8 DS18B20_ID02[8];//DS18B20�¶ȴ�����2 ID
extern u8 DS18B20_ID03[8];//DS18B20�¶ȴ�����3 ID
extern u8 DS18B20_ID04[8];//DS18B20�¶ȴ�����4 ID

//�Զ��庯������
extern void RCC_HSE_Configuration(void); //HSE��ΪPLLʱ�ӣ�PLL��ΪSYSCLK 
extern void RCC_Configuration(void);//ʱ�ӳ�ʼ��
extern void GPIO_Configuration(void);//GPIO��ʼ��
extern void SysTick_Configuration(void);//SysTick��ʼ��
extern void EXIT_Configuraation(void);//�ⲿ�жϳ�ʼ��
extern void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority); //�ж����ȼ���ʼ��

//ucosii���
extern void Delay_Init(void);	//ϵͳ�δ�ʱ�ӳ�ʼ��
extern void delay_us(u32 nus);//ucosii��΢���ʱ
extern void delay_ms(u16 nms);//ucosii�º��뼶��ʱ


#endif
