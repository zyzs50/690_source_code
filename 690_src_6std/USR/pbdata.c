/****************************************************************************
* �ļ����ƣ�pbdata.c
* ˵    ���������ļ�
* ��    �ܣ������CPU�ڲ������ĳ�ʼ���������룬���漰�ⲿ�����ĳ�ʼ��
* ���÷���1��void RCC_Configuration(void)        	ϵͳ������ʱ�ӳ�ʼ��
* ���÷���2��void GPIO_Configuration(void)  			ϵͳ�������Ӧ�Ķ˿ڳ�ʼ��
* ���÷���3��void SysTick_Configuration(void)			ϵͳ�δ�ʱ����ʼ��---�����
* ���÷���4��void Delay_Init(void)	              ϵͳ�δ�ʱ����ʼ��---ucos��
* ���÷���5��void delay_ms(u16 nms)	              ���뼶��ʱ---ucos��
* ���÷���6��void delay_us(u32 nus)	              ΢���ʱ---ucos��
* ���÷���7��void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority)
****************************************************************************/

#include "pbdata.h"
#include "ds18b20.h"
#include "usb_config.h"


//ȫ�ֱ�������
u8 G_Curr_Cmd_Type = 0;//��ǰ��������
u8 G_Pre_Cmd_Type = 0; //֮ǰ��������


u8 G_Computer_Connect_Flag = 0; //��λ�������ɹ���־ 0:��λ��δ���ӣ� 1����λ���Ѿ�����


//0x00000000ָʾģ�鹦���Ƿ�������־,�ֱ�ռ��һλ��   
//bitλ��  0,         1,            2,             3,         4,          5,
//      DS18B20_1, DS18B20_2, ͬ������λ����, ����������, ͬ�������, �˹��ֵ�� 
u8 G_Error_Type = 0;//�����־

OS_EVENT *Func_Sem; //ucosii �ź���
u8 G_Func_Running = 0;//��ʾFUNC��������ִ��ĳ�����
u8 G_Func_Type = 0;//ָʾ��������

u8 G_Power_On_Off = 0;//ָʾ�Ƿ�����Դ
u8 G_Power_On_Off_Fre = 1;//��ԴƵ��


u8 G_Temperature_Disp = 0;//ָʾ�Ƿ���ʾ�¶�
u8 G_Heat1_Start = 0;//ָʾ�Ƿ�ʼ����
u8 G_Heat2_Start = 0;//ָʾ�Ƿ�ʼ����
u8 G_Heat3_Start = 0;//ָʾ�Ƿ�ʼ����
u16 Target_Temperature = 0;

u8 G_Beep_on_off = 0; //����������
u8 G_Beep_count  = 1; //������Ƶ��

u8 DS18B20_ID01[8]={0x28,0x59,0xE0,0xC6,0x09,0x00,0x00,0xC2};//DS18B20�¶ȴ�����1 ID
u8 DS18B20_ID02[8]={0x28,0x50,0xE6,0xC4,0x09,0x00,0x00,0xCF};//DS18B20�¶ȴ�����2 ID
u8 DS18B20_ID03[8]={0x28,0xD2,0x66,0xC6,0x09,0x00,0x00,0x87};//DS18B20�¶ȴ�����3 ID
u8 DS18B20_ID04[8]={0x28,0xA3,0xF2,0xC4,0x09,0x00,0x00,0x87 };//DS18B20�¶ȴ�����4 ID

//-----------------------------------ϵͳ�δ�ʱ���漰���ı���--��ʱ��-----------------------------
u8  fac_us=0;//us��ʱ������
u16 fac_ms=0;//ms��ʱ������

void my_SystemInit (void);


/****************************************************************************
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ�ϵͳʱ�ӳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������
* ���÷������� 
****************************************************************************/
void RCC_Configuration(void)
{
	//ϵͳʱ�ӳ�ʼ��
  RCC_HSE_Configuration();            //ʵ��ʹ�õ����ڲ�����SysclkΪ48M
//	my_SystemInit();
	//ʹ��GPIO������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ�ܶ˿�A��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ�ܶ˿�B��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ�ܶ˿�C��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ�ܶ˿�D��ʱ��
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);//ʹ�ܶ˿�D��ʱ��
	
	
	  
 
	//����IO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //����˿ڳ���Ϊ������������һ�����Ϊ�ж�������������ܣ���Ҫʹ�ܶ˿ڸ��ù���ʱ��
	//UART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ�ܴ���1��ʱ��
	//AD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//ʹ��AD1��ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);//48/4 = 12M  ���14M
	//Timer4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                  
	//USB
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);//PLLCLKʱ�ӷ�Ƶ1�õ�USBʱ��Ƶ�� 48/1=48MHZ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB,ENABLE);//ʹ��USB��ʱ��
	//DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	
}

/****************************************************************************
* ��    �ƣ�void GPIO_Configuration(void)
* ��    �ܣ�GPIO��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������
* ���÷������� 
****************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//��ʼ������0��Ӧ��GPIO�ܽ�
	//����1ʹ�õ���PA9��PA10�� PA9ΪTX������Ϊ�������������PA10ΪRX������Ϊ�������룬
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;			     //ѡ���9����  TX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;	     //�����������
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;			     //ѡ���10����  RX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//��ʼ��AD0��Ӧ��GPIO����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;	     //ģ������
	GPIO_Init(GPIOA,&GPIO_InitStruct);
  //��Դ�ƶ�Ӧ��GPIO����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	GPIO_Init(GPIOA,&GPIO_InitStruct);
  //GPIO_SetBits(GPIOA,GPIO_Pin_8);	//����Դ��
 	GPIO_ResetBits(GPIOA,GPIO_Pin_8);	  //�ع�Դ��
	//USB��Ӧ��GPIO����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;	   //�������
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//����漰��������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹͬ�������ʹ��	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);//AP�˿���0	
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);//CP�˿���0	
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);//DP�˿���0	
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);//BP�˿���0	


/*
	//����Ƭ�漰����������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);//BP�˿���0	����
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//BP�˿���0	����
	GPIO_SetBits(GPIOB,GPIO_Pin_7);//BP�˿���0	����Ƭ
*/	

	//����Ƭ�漰����������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);//BP�˿���0	����
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//BP�˿���0	����
	




	//������������Ӧ������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5;            //�������������� , ����ΪPin_5
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOB,&GPIO_InitStruct);

//�¶ȴ�����1��Ӧ������
	GPIO_InitStruct.GPIO_Pin   = IO_DS18B20_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	GPIO_Init(GPIO_DS18B20_1,&GPIO_InitStruct);
	
 	//�¶ȴ�����2��Ӧ������
  GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);       //ӳ��PD01��ʹ��
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStruct);
	
	
	
	//�˹��ֺ�ͬ���������Ӧ������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//���16λAD����GPIOCΪ��������-��ʱ����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//��������Ӧ������
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	//USB��Ӧ������/* PD.09 used as USB pull-up */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/****************************************************************************
* ��    �ƣ�void SysTick_Configuration(void)
* ��    �ܣ�SysTick��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����ʱ��ԴΪAHBCLK �� 8��Ƶ�� ��ʱ1ms�����жϣ��������ж�ģʽ
* ���÷�������Ϊ�����ϵͳ����ʱ�������ôκ�����ʼ��SysTick
****************************************************************************/
void SysTick_Configuration(void)
{
// 	SysTick_TypeDef SysTick_Struct;
  	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//ABHʱ��8��Ƶ  9MHZ
// 	SysTick_SetReload(9000);//1ms����
//   SysTick_ITConfig(ENABLE);//	ʹ�ܶ�ʱ���ж�
// 	SysTick_CounterCmd(SysTick_Counter_Enable);//ʹ�ܼ���
	 SysTick->LOAD = 9000;
	 SysTick->VAL=0X00;//��ռ�����
// 	 NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	 SysTick->CTRL=0X03;//ʱ��Դѡ��8��Ƶ��ʹ�ܼ����жϣ�ʹ�ܶ�ʱ��
	
	
}
/****************************************************************************
* ��    �ƣ�void EXIT_Configuraation(void)
* ��    �ܣ��ⲿ�жϳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ 1.ͨ��I/O �˿����ӵ�16 ���ⲿ�ж�/�¼����ϣ�
*              2.���ж˿ڵĵ�0�ܽŶ����� �ⲿ�ж�0 EXTI0��
*              3.���ж˿ڵĵ�1�ܽŶ����� �ⲿ�ж�1 EXTI1��
*              4.���ж˿ڵĵ�1�ܽŶ����� �ⲿ�ж�2 EXTI2��......
* ���÷������� 
****************************************************************************/
void EXIT_Configuraation(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_ClearITPendingBit(EXTI_Line5);//����ⲿ�ж�״��EXTI5�Ĺ���״̬
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5); //��GPIOA�ĵ�5�ܽ������ⲿ�ж���·

	EXTI_InitStructure.EXTI_Line=EXTI_Line5;//ȷ�������ⲿ�ж��ߣ���Ϊ�ⲿ�ж���ΪEXTI5
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;//�ж�����ģʽ
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;//ʹ����·״̬

	EXTI_Init(&EXTI_InitStructure);
}

/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority)
* ��    �ܣ��ж���Ӧ���ȼ���ʼ��
* ��ڲ���1�����ȼ�����
* ��ڲ���2���ж�ͨ��
* ��ڲ���3����ռ���ȼ�
* ��ڲ���4����Ӧ���ȼ�
* ���ڲ�������
* ˵    ������ TIM_TimeBaseInit
* ���÷������� 
****************************************************************************/
void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority)
{
  NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_PriorityGroupConfig(PriorityGroup); //���ȼ����飬�ܹ���5�飬����ѡ���1�飬��ռ���ȼ�ռ1λ����Ӧ���ȼ�ռ3λ

	NVIC_InitStructure.NVIC_IRQChannel = IRQChannel; //ѡ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority; //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority; //��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
}


/****************************************************************************
* ��    �ƣ�void RCC_HSE_Configuration(void)
* ��    �ܣ��Զ���ʱ�Ӻ��� �� ����ʹ�õ����ڲ�8MHZ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����SystemInit()�����Ǳ�׼������ĺ������Ѿ������˳�ʼ��ϵͳʱ�ӡ�
*						���������Ҫ�Զ����ʱ�ӣ��������д�Զ����ʱ�Ӻ���������Ҫ�ڿ⺯���������޸ġ�
* ���÷������κ����Ĺ��� �� SystemInit()һ��
****************************************************************************/
void RCC_HSE_Configuration(void) //HSE��ΪPLLʱ�ӣ�PLL��ΪSYSCLK 
{ 
   RCC_DeInit(); /*������RCC�Ĵ�������Ϊȱʡֵ */ 
 	 RCC_HSICmd(ENABLE);
   while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)== RESET);//�ȴ�HSI����	
	 RCC_HCLKConfig(RCC_SYSCLK_Div1);/*����AHBʱ��(HCLK)RCC_SYSCLK_Div1����AHBʱ�� = ϵͳʱ*/  //      48
	 RCC_PCLK2Config(RCC_HCLK_Div1); /*���ø���AHBʱ��(PCLK2)RCC_HCLK_Div1����APB2ʱ�� = HCLK*/ //     48
	 RCC_PCLK1Config(RCC_HCLK_Div2); /*���õ���AHBʱ��(PCLK1)RCC_HCLK_Div2����APB1ʱ�� = HCLK / 2*/ // 24    
	 FLASH_SetLatency(FLASH_Latency_2);	//FLASH_Latency_2 2��ʱ����
	 FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//Ԥȡָ����ʹ��
		//��Ϊʹ��USB�ӿڣ�����PLL�����ʱ�ӱ������� 48MHZ �� 72MHZ
	 RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);/*����PLLʱ��Դ����Ƶϵ����Ƶ��Ϊ8/2*12=48Mhz*/    
	 RCC_PLLCmd(ENABLE); 	 /*ʹ��PLL */ 
	 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*���ָ����RCC��־λ(PLL׼���ñ�־)�������*/    
	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*����ϵͳʱ�ӣ�SYSCLK�� */  
	 while(RCC_GetSYSCLKSource() != 0x08);  	
	
	//  RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);//4*16 = 64MHZ
	 
// 	 RCC_PLLCmd(ENABLE);
// 	 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*���ָ����RCC��־λ(PLL׼���ñ�־)�������*/ 
// 	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*����PLL��Ϊϵͳʱ�ӣ�SYSCLK�� */  //               
// 	 while(RCC_GetSYSCLKSource() != 0x08);     /*0x08���ȴ�PLL��Ϊϵͳʱ�ӵ�״̬�Ѿ���� */	    
	
//    if(RCC_WaitForHSEStartUp() == SUCCESS)/*�ȴ�HSE����,  SUCCESS��HSE�����ȶ��Ҿ���*/  
// 	 {  

// 		 RCC_HCLKConfig(RCC_SYSCLK_Div1);/*����AHBʱ��(HCLK)RCC_SYSCLK_Div1����AHBʱ�� = ϵͳʱ*/  
// 		 RCC_PCLK2Config(RCC_HCLK_Div1); /*���ø���AHBʱ��(PCLK2)RCC_HCLK_Div1����APB2ʱ�� = HCLK*/     
// 		 RCC_PCLK1Config(RCC_HCLK_Div2); /*���õ���AHBʱ��(PCLK1)RCC_HCLK_Div2����APB1ʱ�� = HCLK / 2*/      

// 		 RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);/*����PLLʱ��Դ����Ƶϵ����RCC_PLLSource_HSE_Div1�����ⲿ����HSE�Ƿ��Ƶ*/    
// 		 RCC_PLLCmd(ENABLE); 	 /*ʹ��PLL */ 
// 		 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*���ָ����RCC��־λ(PLL׼���ñ�־)�������*/ 
// 			
// 		 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*����PLL��Ϊϵͳʱ�ӣ�SYSCLK�� */  
// 		 while(RCC_GetSYSCLKSource() != 0x08);     /*0x08���ȴ�PLL��Ϊϵͳʱ�ӵ�״̬�Ѿ���� */	    
// 	 } 
} 
/****************************************************************************
* ��    �ƣ�void my_SystemInit(void)
* ��    �ܣ��Զ���ʱ�Ӻ��� �� ����ʹ�õ����ڲ�8MHZ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void my_SystemInit (void)
{
    RCC_DeInit(); /*������RCC�Ĵ�������Ϊȱʡֵ */ 
		/* Set HSION bit */
		RCC->CR |= (uint32_t)0x00000001;	
		// select HSI as PLL source
		RCC->CFGR |= (uint32_t)RCC_CFGR_PLLSRC_HSI_Div2;        
		
		//PLLCLK=8/2*12=48M
		RCC->CFGR |= (uint32_t)RCC_CFGR_PLLMULL12;
				
		 /* HCLK = SYSCLK/4      */
		RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV4;
		 
				/* Enable PLL */
		RCC->CR |= RCC_CR_PLLON;
				
		/* Wait till PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}
		/* Select PLL as system clock source */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    			
		/* Wait till PLL is used as system clock source */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
		{
		}

}

/****************************************************************************
* ��    �ƣ�void Delay_Init(void)
* ��    �ܣ�ϵͳ�δ�ʱ�ӳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ��ģ��ú���ʵ��5ms�Ķ�ʱ�ж�
* ���÷�����
****************************************************************************/
void Delay_Init(void)	 
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ�� HCLK/8���� SysTick_CLK = 48/8 = 6MHZ 
	fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  ����fac_us = 6����1us��Ҫ6����
	 
	reload=SystemCoreClock/8000000;	//48000000/8000000=6 �൱�� 1us ����6��   
	reload*=1000000/OS_TICKS_PER_SEC;//����OS_TICKS_PER_SEC�趨���ʱ�� �� ��ʱreload = 6*5000 �Σ� ����6 = 5000us=5ms�� �� ��ʱ���ж�Ϊ5ms
									                 //reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	fac_ms=1000/OS_TICKS_PER_SEC;//����ucos������ʱ�����ٵ�λ	   ����fac_ms = 5
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	����5ms�ж�һ��
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    
}								    

/****************************************************************************
* ��    �ƣ�void delay_us(u32 nus)
* ��    �ܣ�΢���ʱ
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʹ��ucos��ʱ��,�˺�����Ϊ΢���ʱ
* ���÷�����΢���ʱ�����ò�ѯ�ĵķ�ʽ��
****************************************************************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;	//LOAD��ֵ	    	 
	ticks=nus*fac_us; 			//��Ҫ�Ľ�����	  		 
	tcnt=0;
	told=SysTick->VAL;        	//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	      //��ǰ����ֵ
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else 
				tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}; 									    
}

/****************************************************************************
* ��    �ƣ�void delay_ms(u16 nms)
* ��    �ܣ����뼶��ʱ
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʹ��ucos��ʱ��,�˺�����Ϊ���뼶��ʱ
* ���÷�����fac_ms = 5 ��
****************************************************************************/
void delay_ms(u16 nms)
{	
	if(OSRunning==TRUE)//���os�Ѿ�������	    
	{		  
		if(nms>=fac_ms)//��ʱ��ʱ�����ucos������ʱ������ 
		{
   			OSTimeDly(nms/fac_ms);//ucos��ʱ
		}
		nms%=fac_ms;				//ucos�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));	//��ͨ��ʽ��ʱ,��ʱucos�޷���������.
}


/****************************************************************************
* ��    �ƣ�int fputc(int ch,FILE *f)
* ��    �ܣ�fputc��������
* ��ڲ�������
* ���ڲ�������
* ˵    ����ִ��printf����ʱ������ô˺���
* ���÷��������ڿ���ֱ��ʹ��printf�����Ӵ���1����ַ�
****************************************************************************/
// int fputc(int ch,FILE *f)
// {
// 	USART_SendData(USART1,(u8)ch);
// 	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
// 	{
// 	}
// 	return ch;
// }

int fputc(int ch,FILE *f)
{
	u8 Response_CMD[1]={0x00};

	Response_CMD[0]=(u8)ch;
	USB_TxWrite(Response_CMD, 1);//����1�ֽ�
	return ch;
}
