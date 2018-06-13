/****************************************************************************
* 文件名称：pbdata.c
* 说    明：共用文件
* 功    能：里面放CPU内部独立的初始化函数代码，不涉及外部器件的初始化
* 内置方法1：void RCC_Configuration(void)        	系统各部件时钟初始化
* 内置方法2：void GPIO_Configuration(void)  			系统各外设对应的端口初始化
* 内置方法3：void SysTick_Configuration(void)			系统滴答定时器初始化---裸机用
* 内置方法4：void Delay_Init(void)	              系统滴答定时器初始化---ucos用
* 内置方法5：void delay_ms(u16 nms)	              毫秒级延时---ucos用
* 内置方法6：void delay_us(u32 nus)	              微妙级延时---ucos用
* 内置方法7：void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority)
****************************************************************************/

#include "pbdata.h"
#include "ds18b20.h"
#include "usb_config.h"


//全局变量定义
u8 G_Curr_Cmd_Type = 0;//当前命令类型
u8 G_Pre_Cmd_Type = 0; //之前命令类型


u8 G_Computer_Connect_Flag = 0; //上位机联机成功标志 0:上位机未连接， 1：上位机已经连接


//0x00000000指示模块功能是否正常标志,分别占据一位：   
//bit位：  0,         1,            2,             3,         4,          5,
//      DS18B20_1, DS18B20_2, 同步带定位光耦, 霍尔传感器, 同步带电机, 滤光轮电机 
u8 G_Error_Type = 0;//错误标志

OS_EVENT *Func_Sem; //ucosii 信号量
u8 G_Func_Running = 0;//表示FUNC任务正在执行某命令动作
u8 G_Func_Type = 0;//指示哪种任务

u8 G_Power_On_Off = 0;//指示是否开启灯源
u8 G_Power_On_Off_Fre = 1;//灯源频率


u8 G_Temperature_Disp = 0;//指示是否显示温度
u8 G_Heat1_Start = 0;//指示是否开始加热
u8 G_Heat2_Start = 0;//指示是否开始加热
u8 G_Heat3_Start = 0;//指示是否开始加热
u16 Target_Temperature = 0;

u8 G_Beep_on_off = 0; //蜂鸣器开关
u8 G_Beep_count  = 1; //蜂鸣器频率

u8 DS18B20_ID01[8]={0x28,0x59,0xE0,0xC6,0x09,0x00,0x00,0xC2};//DS18B20温度传感器1 ID
u8 DS18B20_ID02[8]={0x28,0x50,0xE6,0xC4,0x09,0x00,0x00,0xCF};//DS18B20温度传感器2 ID
u8 DS18B20_ID03[8]={0x28,0xD2,0x66,0xC6,0x09,0x00,0x00,0x87};//DS18B20温度传感器3 ID
u8 DS18B20_ID04[8]={0x28,0xA3,0xF2,0xC4,0x09,0x00,0x00,0x87 };//DS18B20温度传感器4 ID

//-----------------------------------系统滴答时钟涉及到的变量--延时用-----------------------------
u8  fac_us=0;//us延时倍乘数
u16 fac_ms=0;//ms延时倍乘数

void my_SystemInit (void);


/****************************************************************************
* 名    称：void RCC_Configuration(void)
* 功    能：系统时钟初始化
* 入口参数：无
* 出口参数：无
* 说    明：无
* 调用方法：无 
****************************************************************************/
void RCC_Configuration(void)
{
	//系统时钟初始化
  RCC_HSE_Configuration();            //实际使用的是内部晶振，Sysclk为48M
//	my_SystemInit();
	//使能GPIO及外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能端口A的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能端口B的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能端口C的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能端口D的时钟
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);//使能端口D的时钟
	
	
	  
 
	//复用IO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //如果端口除作为输入输出，而且还能作为中断输入或其它功能，还要使能端口复用功能时钟
	//UART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能串口1的时钟
	//AD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//使能AD1的时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);//48/4 = 12M  最大14M
	//Timer4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                  
	//USB
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);//PLLCLK时钟分频1得到USB时钟频率 48/1=48MHZ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB,ENABLE);//使能USB的时钟
	//DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	
}

/****************************************************************************
* 名    称：void GPIO_Configuration(void)
* 功    能：GPIO初始化
* 入口参数：无
* 出口参数：无
* 说    明：无
* 调用方法：无 
****************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//初始化串口0对应的GPIO管脚
	//串口1使用的是PA9，PA10， PA9为TX：配置为复用推挽输出，PA10为RX：配置为浮空输入，
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;			     //选择第9引脚  TX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;	     //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;			     //选择第10引脚  RX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//初始化AD0对应的GPIO引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;	     //模拟输入
	GPIO_Init(GPIOA,&GPIO_InitStruct);
  //光源灯对应的GPIO引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStruct);
  //GPIO_SetBits(GPIOA,GPIO_Pin_8);	//开光源灯
 	GPIO_ResetBits(GPIOA,GPIO_Pin_8);	  //关光源灯
	//USB对应的GPIO引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;	   //推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//电机涉及到的引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止同步带电机使能	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);//AP端口清0	
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);//CP端口清0	
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);//DP端口清0	
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);//BP端口清0	


/*
	//加热片涉及帕贴的引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);//BP端口清0	帕贴
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//BP端口清0	帕贴
	GPIO_SetBits(GPIOB,GPIO_Pin_7);//BP端口清0	加热片
*/	

	//加热片涉及帕贴的引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);//BP端口清0	帕贴
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//BP端口清0	帕贴
	




	//霍尔传感器对应的引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5;            //读霍尔传感器点 , 更改为Pin_5
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStruct);

//温度传感器1对应的引脚
	GPIO_InitStruct.GPIO_Pin   = IO_DS18B20_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_Init(GPIO_DS18B20_1,&GPIO_InitStruct);
	
 	//温度传感器2对应的引脚
  GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);       //映射PD01的使能
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStruct);
	
	
	
	//滤光轮和同步带光耦对应的引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//外接16位AD配置GPIOC为浮空输入-暂时不用
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//蜂鸣器对应的引脚
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	//USB对应的引脚/* PD.09 used as USB pull-up */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/****************************************************************************
* 名    称：void SysTick_Configuration(void)
* 功    能：SysTick初始化
* 入口参数：无
* 出口参数：无
* 说    明：时钟源为AHBCLK 的 8分频， 定时1ms产生中断，工作于中断模式
* 调用方法：当为裸机无系统环境时，可以用次函数初始化SysTick
****************************************************************************/
void SysTick_Configuration(void)
{
// 	SysTick_TypeDef SysTick_Struct;
  	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//ABH时钟8分频  9MHZ
// 	SysTick_SetReload(9000);//1ms计数
//   SysTick_ITConfig(ENABLE);//	使能定时器中断
// 	SysTick_CounterCmd(SysTick_Counter_Enable);//使能计数
	 SysTick->LOAD = 9000;
	 SysTick->VAL=0X00;//清空计数器
// 	 NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	 SysTick->CTRL=0X03;//时钟源选择8分频，使能计数中断，使能定时器
	
	
}
/****************************************************************************
* 名    称：void EXIT_Configuraation(void)
* 功    能：外部中断初始化
* 入口参数：无
* 出口参数：无
* 说    明：无 1.通用I/O 端口连接到16 个外部中断/事件线上：
*              2.所有端口的第0管脚都连在 外部中断0 EXTI0上
*              3.所有端口的第1管脚都连在 外部中断1 EXTI1上
*              4.所有端口的第1管脚都连在 外部中断2 EXTI2上......
* 调用方法：无 
****************************************************************************/
void EXIT_Configuraation(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_ClearITPendingBit(EXTI_Line5);//清除外部中断状线EXTI5的挂起状态
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5); //将GPIOA的第5管脚用作外部中断线路

	EXTI_InitStructure.EXTI_Line=EXTI_Line5;//确定哪条外部中断线，此为外部中断线为EXTI5
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;//中断请求模式
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;//使能线路状态

	EXTI_Init(&EXTI_InitStructure);
}

/****************************************************************************
* 名    称：void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority)
* 功    能：中断响应优先级初始化
* 入口参数1：优先级分组
* 入口参数2：中断通道
* 入口参数3：抢占优先级
* 入口参数4：响应优先级
* 出口参数：无
* 说    明：无 TIM_TimeBaseInit
* 调用方法：无 
****************************************************************************/
void NVIC_Configuration(uint32_t PriorityGroup,uint32_t IRQChannel,uint32_t PreemptionPriority,uint32_t SubPriority)
{
  NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_PriorityGroupConfig(PriorityGroup); //优先级分组，总共有5组，这里选择第1组，抢占优先级占1位，响应优先级占3位

	NVIC_InitStructure.NVIC_IRQChannel = IRQChannel; //选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority; //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority; //响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能中断通道
	NVIC_Init(&NVIC_InitStructure);
}


/****************************************************************************
* 名    称：void RCC_HSE_Configuration(void)
* 功    能：自定义时钟函数 ， 这里使用的是内部8MHZ晶振
* 入口参数：无
* 出口参数：无
* 说    明：SystemInit()函数是标准库里面的函数，已经设置了初始的系统时钟。
*						但是如果想要自定义的时钟，最好重新写自定义的时钟函数，而不要在库函数基础上修改。
* 调用方法：次函数的功能 和 SystemInit()一样
****************************************************************************/
void RCC_HSE_Configuration(void) //HSE作为PLL时钟，PLL作为SYSCLK 
{ 
   RCC_DeInit(); /*将外设RCC寄存器重设为缺省值 */ 
 	 RCC_HSICmd(ENABLE);
   while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)== RESET);//等待HSI就绪	
	 RCC_HCLKConfig(RCC_SYSCLK_Div1);/*设置AHB时钟(HCLK)RCC_SYSCLK_Div1——AHB时钟 = 系统时*/  //      48
	 RCC_PCLK2Config(RCC_HCLK_Div1); /*设置高速AHB时钟(PCLK2)RCC_HCLK_Div1——APB2时钟 = HCLK*/ //     48
	 RCC_PCLK1Config(RCC_HCLK_Div2); /*设置低速AHB时钟(PCLK1)RCC_HCLK_Div2——APB1时钟 = HCLK / 2*/ // 24    
	 FLASH_SetLatency(FLASH_Latency_2);	//FLASH_Latency_2 2延时周期
	 FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//预取指缓存使能
		//因为使用USB接口，所以PLL的输出时钟必须满足 48MHZ 或 72MHZ
	 RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);/*设置PLL时钟源及倍频系数，频率为8/2*12=48Mhz*/    
	 RCC_PLLCmd(ENABLE); 	 /*使能PLL */ 
	 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*检查指定的RCC标志位(PLL准备好标志)设置与否*/    
	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*设置系统时钟（SYSCLK） */  
	 while(RCC_GetSYSCLKSource() != 0x08);  	
	
	//  RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);//4*16 = 64MHZ
	 
// 	 RCC_PLLCmd(ENABLE);
// 	 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*检查指定的RCC标志位(PLL准备好标志)设置与否*/ 
// 	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*设置PLL作为系统时钟（SYSCLK） */  //               
// 	 while(RCC_GetSYSCLKSource() != 0x08);     /*0x08：等待PLL作为系统时钟的状态已经完成 */	    
	
//    if(RCC_WaitForHSEStartUp() == SUCCESS)/*等待HSE起振,  SUCCESS：HSE晶振稳定且就绪*/  
// 	 {  

// 		 RCC_HCLKConfig(RCC_SYSCLK_Div1);/*设置AHB时钟(HCLK)RCC_SYSCLK_Div1——AHB时钟 = 系统时*/  
// 		 RCC_PCLK2Config(RCC_HCLK_Div1); /*设置高速AHB时钟(PCLK2)RCC_HCLK_Div1——APB2时钟 = HCLK*/     
// 		 RCC_PCLK1Config(RCC_HCLK_Div2); /*设置低速AHB时钟(PCLK1)RCC_HCLK_Div2——APB1时钟 = HCLK / 2*/      

// 		 RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);/*设置PLL时钟源及倍频系数，RCC_PLLSource_HSE_Div1代表外部晶振HSE是否分频*/    
// 		 RCC_PLLCmd(ENABLE); 	 /*使能PLL */ 
// 		 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) ; /*检查指定的RCC标志位(PLL准备好标志)设置与否*/ 
// 			
// 		 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /*设置PLL作为系统时钟（SYSCLK） */  
// 		 while(RCC_GetSYSCLKSource() != 0x08);     /*0x08：等待PLL作为系统时钟的状态已经完成 */	    
// 	 } 
} 
/****************************************************************************
* 名    称：void my_SystemInit(void)
* 功    能：自定义时钟函数 ， 这里使用的是内部8MHZ晶振
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void my_SystemInit (void)
{
    RCC_DeInit(); /*将外设RCC寄存器重设为缺省值 */ 
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
* 名    称：void Delay_Init(void)
* 功    能：系统滴答时钟初始化
* 入口参数：无
* 出口参数：无
* 说    明：当使用ucos的时候,此函数会初始化ucos的时钟节拍，该函数实现5ms的定时中断
* 调用方法：
****************************************************************************/
void Delay_Init(void)	 
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟 HCLK/8，即 SysTick_CLK = 48/8 = 6MHZ 
	fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8  这里fac_us = 6，即1us需要6节拍
	 
	reload=SystemCoreClock/8000000;	//48000000/8000000=6 相当于 1us 计数6次   
	reload*=1000000/OS_TICKS_PER_SEC;//根据OS_TICKS_PER_SEC设定溢出时间 ， 此时reload = 6*5000 次， 除以6 = 5000us=5ms， 即 定时器中断为5ms
									                 //reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/OS_TICKS_PER_SEC;//代表ucos可以延时的最少单位	   这里fac_ms = 5
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	，即5ms中断一次
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    
}								    

/****************************************************************************
* 名    称：void delay_us(u32 nus)
* 功    能：微妙级延时
* 入口参数：无
* 出口参数：无
* 说    明：当使用ucos的时候,此函数作为微妙级延时
* 调用方法：微妙级延时，采用查询的的方式，
****************************************************************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;	//LOAD的值	    	 
	ticks=nus*fac_us; 			//需要的节拍数	  		 
	tcnt=0;
	told=SysTick->VAL;        	//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	      //当前计数值
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else 
				tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
		}  
	}; 									    
}

/****************************************************************************
* 名    称：void delay_ms(u16 nms)
* 功    能：毫秒级延时
* 入口参数：无
* 出口参数：无
* 说    明：当使用ucos的时候,此函数作为毫秒级延时
* 调用方法：fac_ms = 5 ，
****************************************************************************/
void delay_ms(u16 nms)
{	
	if(OSRunning==TRUE)//如果os已经在跑了	    
	{		  
		if(nms>=fac_ms)//延时的时间大于ucos的最少时间周期 
		{
   			OSTimeDly(nms/fac_ms);//ucos延时
		}
		nms%=fac_ms;				//ucos已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));	//普通方式延时,此时ucos无法启动调度.
}


/****************************************************************************
* 名    称：int fputc(int ch,FILE *f)
* 功    能：fputc函数重载
* 入口参数：无
* 出口参数：无
* 说    明：执行printf函数时，会调用此函数
* 调用方法：后期可以直接使用printf函数从串口1输出字符
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
	USB_TxWrite(Response_CMD, 1);//发送1字节
	return ch;
}
