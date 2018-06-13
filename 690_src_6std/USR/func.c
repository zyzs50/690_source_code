/****************************************************************************
* 文件名称：func.c
* 说    明：任务文件
* 功    能：里面主要放任务逻辑动作函数
* 内置方法1：
* 内置方法2：
* 内置方法3：
* 内置方法4：
* 内置方法5：
* 内置方法6：
* 内置方法7：
* 内置方法8：
* 内置方法9：
* 内置方法10：
* 内置方法11：
****************************************************************************/
#include "func.h"
#include "uart.h"
#include "crc32.h"
#include "motor.h"
#include "ds18b20.h"
#include "heat.h"
#include "usb_config.h"
#include "ad.h"
#include "calibrate.h"
#include "pbdata.h"
#include "flash.h"


/****************************************************************************
* 名    称：u8 Func_Front_Connect_Test(void)
* 功    能：给主机返回联机成功命令
* 入口参数：void
* 出口参数：返回自检成功标志
* 说    明：
* 调用方法：主机发 03 00 00 00 00 00 fd c1 b4 4c ff  ，前端板 回应 30 ff
****************************************************************************/
u8 Func_Front_Connect_Test(void)
{
	u8 Response_CMD[3]={0x30,0x00,0xff};
	
	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Front_Self_Test(void)
* 功    能：自检任务函数
* 入口参数：void
* 出口参数：返回自检成功标志
* 说    明：
* 调用方法：主机发 04 
****************************************************************************/
u8 Func_Front_Self_Test(void)
{
	u8 Response_CMD[3]={0x40,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	G_Computer_Connect_Flag = 1;//表示已经成功连接上 上位机
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Sample_Test_Prepare(void)
* 功    能：样品准备测试
* 入口参数：void
* 出口参数：返回样品准备测试标志
* 说    明：
* 调用方法：主机发 06 
****************************************************************************/
u8 Func_Sample_Test_Prepare(void)
{
	u8 Response_CMD[3]={0x60,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}
/****************************************************************************
* 名    称：u8 Func_Sample_Test_Start(void)
* 功    能：开始样本测试
* 入口参数：void
* 出口参数：返回样本测试标志
* 说    明：
* 调用方法：主机发 07
****************************************************************************/
u8 Func_Sample_Test_Start(void)
{
	u8 Response_CMD[3]={0x70,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Sample_Test_Stop(void)
* 功    能：测样停止
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：08
****************************************************************************/
u8 Func_Sample_Test_Stop(void)
{
	u8 Response_CMD[3]={0x80,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Com_On(void)
* 功    能：串口启动
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：09
****************************************************************************/
u8 Func_Com_On(void)
{
	u8 Response_CMD[3]={0x90,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Com_Off(void)
* 功    能：串口关闭
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：0A
****************************************************************************/
u8 Func_Com_Off(void)
{
	u8 Response_CMD[3]={0xA0,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_USB_On(void)
* 功    能：USB启动
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：0B
****************************************************************************/
u8 Func_USB_On(void)
{
	u8 Response_CMD[3]={0xB0,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_USB_Off(void)
* 功    能：USB关闭
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：0C
****************************************************************************/
u8 Func_USB_Off(void)
{
	u8 Response_CMD[3]={0xC0,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Light_On(void)
* 功    能：灯源开启
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：0D
****************************************************************************/
u8 Func_Light_On(void)
{
	u8 Response_CMD[3]={0xd0,0x00,0xff};

	GPIO_SetBits(GPIOA,GPIO_Pin_8);	//开光源灯

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Light_Off(u8 fre)
* 功    能：灯源关闭
* 入口参数：u8 fre
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：0E
****************************************************************************/
u8 Func_Light_Off(u8 fre)
{
	u8 Response_CMD[3]={0xE0,0x00,0xff};

	if(fre==0)
	{
		G_Power_On_Off = 0;//关闭光源任务
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//关闭光源灯
	}
	else
	{
		G_Power_On_Off = 1;//开启光源任务
		G_Power_On_Off_Fre = fre;//亮度强度
		
	}

	USB_TxWrite(Response_CMD, 3);//发送响应



	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Temperature_Disp(void)
* 功    能：温育开启
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：13
****************************************************************************/
// u8 Func_Temperature_Disp(void)
// {
// 	u8 i,j;
// 	u16 temp=0;
// 	u8 Response_CMD[3]={0x31,0x00,0xff};

// 	USB_TxWrite(Response_CMD, 3);//发送响应

//   
//   //输出ID
// 	for(i=0;i<G_Ds18b20_count;i++)
// 	{
// 		printf("18B20 ROM_ID %d = %X %X %X %X %X %X %X %X \r\n",i,G_DS18B20_ID[i][0],G_DS18B20_ID[i][1],G_DS18B20_ID[i][2],G_DS18B20_ID[i][3],G_DS18B20_ID[i][4],G_DS18B20_ID[i][5],G_DS18B20_ID[i][6],G_DS18B20_ID[i][7]);
// 		delay_ms(50);
// 	}
// 	//前几次偏差比较大，舍弃
// 	for(i=0;i<5;i++)
// 	{
// 		for(j=0;j<G_Ds18b20_count;j++)
// 		{
// 			temp=DS18B20_Get_Temp(G_DS18B20_ID[j]);
// 			delay_ms(30);
// 		}
// 	}

// 	//输出温度
// 	for(i=0;i<20;i++)
// 	{
// 		printf("---------%d----------\r\n",i);  	  
// 		for(j=0;j<G_Ds18b20_count;j++)
// 		{
// 			temp=DS18B20_Get_Temp(G_DS18B20_ID[j]);
// 			printf("temp = %d\r\n",temp);
// 			temp = Temperature_convert(temp);
// 			if(temp&0x8000)//负数
// 			{
// 				temp = temp &0x7fff;
// 				printf("current tenperature ID-%d: -%d.%d \r\n",j,temp>>8,temp&0xff);  	  
// 			}
// 			else
// 			{
// 				printf("current tenperature ID-%d: %d.%d  \r\n",j,temp>>8,temp&0xff);  	  
// 			}
// 			delay_ms(500);
// 		}
// 		
// 		if(G_Temperature_Disp ==0)
// 		{
// 			break;
// 		}
// 	}

// 	return 0;
// }

/****************************************************************************
* 名    称：u8 Func_Temperature_Hot(u16 Target)
* 功    能：温育加热
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：12 - 02
****************************************************************************/
u8 Func_Temperature_Hot(u16 Target)
{
	//uint16_t i =0;
	//uint8_t j =0;
 	//uint8_t k =0;
 	u16 cur_temp=0;
	u8 Response_CMD[3]={0x31,0x00,0xff};
	//初始化加热片
	Heat3_Init();
	G_Heat3_Start =1;//开始加热
	
	//初始化帕贴加热
	Peltier_Init(1);
	G_Heat1_Start =1;//帕贴开始加热
	G_Heat2_Start = 0;//帕贴制冷关闭
	//USB_TxWrite(Response_CMD, 3);//发送响应


	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Temperature_Cold(u16 Target)
* 功    能：温育制冷
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：12 - 03
****************************************************************************/
u8 Func_Temperature_Cold(u16 Target)
{
	//uint16_t i =0;
	//uint8_t j =0;
 	//uint8_t k =0;
 	//u16 temp=0;
	u8 Response_CMD[3]={0x31,0x00,0xff};
	Peltier_Init(3);
	G_Heat2_Start =1; //帕贴开始制冷
	G_Heat1_Start = 0;//帕贴加热关闭

	if(G_Heat3_Start ==1)//如果加热片处于工作状态，则让加热片待机
	{
		Heat3_Control(Target,IDLESTATE);  //加热片停止加热,输出持续高电平
		G_Heat3_Start = 0;
	}


	return 0;
}
/****************************************************************************
* 名    称：u8 Func_Temperature_Idle(u16 Target)
* 功    能：温育待机
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：12 - 04
****************************************************************************/
u8 Func_Temperature_Idle(u16 Target)
{
	u8 Response_CMD[3]={0x41,0x00,0xff};

	//USB_TxWrite(Response_CMD, 3);//发送响应
  Heat3_Control(Target,IDLESTATE);  //加热片停止加热,输出持续高电平
	
	if(G_Heat1_Start ==1)//帕贴加热状态
	{
		Heat1_Control(Target,IDLESTATE);  //帕尔帖停止加热,输出持续低电平
	}
	
	if(G_Heat2_Start ==1)//帕贴制冷状态
	{
		Heat2_Control(Target,IDLESTATE);  //帕尔帖停止制冷,输出持续低电平
	}
  G_Heat1_Start = 0; 
  G_Heat2_Start = 0; 
  G_Heat3_Start = 0; 
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Temperature_Heat(u16 Target)
* 功    能：温育加热
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：13
****************************************************************************/
u8 Func_Temperature_Heat(u16 Target)
{
	//uint16_t i =0;
	//uint8_t j =0;
 	//uint8_t k =0;
 	//u16 temp=0;
	u8 Response_CMD[3]={0x31,0x00,0xff};
  Heat3_Control(Target,HOTSTATE);

	//USB_TxWrite(Response_CMD, 3);//发送响应


	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Temperature_Off(void)
* 功    能：温育关闭
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：14
****************************************************************************/
u8 Func_Temperature_Off(void)
{
	u8 Response_CMD[3]={0x41,0x00,0xff};
	GPIO_InitTypeDef GPIO_InitStructure;

	TIM_Cmd(TIM4, DISABLE); 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;      //6-->heat2  8-->heat1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);               //低电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);               //低电平

	GPIO_SetBits(GPIOB,GPIO_Pin_7);                 //高电平--加热片
  G_Heat1_Start = 0; 
  G_Heat2_Start = 0; 
  G_Heat3_Start = 0; 
	return 0;
}



/****************************************************************************
* 名    称：u8 Func_Peltier_Board_Heat(void)
* 功    能：帕贴制热
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：15
****************************************************************************/
u8 Func_Peltier_Board_Heat(u16 Target)
{
	uint8_t j =0;
	u8 Response_CMD[3]={0x51,0x00,0xff};

	//USB_TxWrite(Response_CMD, 3);//发送响应

	Heat1_Control(Target,HOTSTATE);

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Peltier_Board_Cold(void)
* 功    能：帕贴制冷
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：16
****************************************************************************/
u8 Func_Peltier_Board_Cold(u16 Target)
{
	uint8_t j =0;
	u8 Response_CMD[3]={0x61,0x00,0xff};

// 	USB_TxWrite(Response_CMD, 3);//发送响应

  Heat2_Control(Target,COLDSTATE);
	return 0;
}



/****************************************************************************
* 名    称：u8 Func_Plate_Motor_Reset(u8 speed)
* 功    能：同步带电机运行复位
* 入口参数：
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：17
****************************************************************************/
u8 Func_Plate_Motor_Reset(u8 speed)
{
	u8 Response_CMD[3]={0x71,0x00,0xff};
	u8 ret =0;

	if(speed<=0)
	{
		return 1;
	}
	
	ret = Plate_Motor_Reset(speed);
	Response_CMD[1]=ret;
	USB_TxWrite(Response_CMD, 3);//发送响应
	
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Plate_Motor_Run(uint16_t speed,uint16_t step)
* 功    能：同步带电机运行指定step
* 入口参数：speed ：运行速率
* 入口参数：step  ：运行多少步
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：电机一步1.8度，200步一圈 ，总行程600 ，step 不要超过6
* 调用方法：主机发送：18
****************************************************************************/
u8 Func_Plate_Motor_Run(uint16_t speed,uint16_t step)
{
	u8 Response_CMD[3]={0x81,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

  Plate_Motor_Run(0,speed,step);//进仓
  Plate_Motor_Run(1,speed,step);//出仓
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Plate_Motor_Position(u8 index)
* 功    能：同步带电机定位
* 入口参数：step ：运行多少步
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：电机一步1.8度，200步一圈
* 调用方法：主机发送：19
****************************************************************************/
u8 Func_Plate_Motor_Position(u8 speed,u8 index)
{
	u8 Response_CMD[3]={0x91,0x00,0xff};
	u8 ret =0;
	ret = Plate_Motor_Position(speed,index);
	Response_CMD[1]=ret;

	USB_TxWrite(Response_CMD, 3);//发送响应
	return 0;
}


/****************************************************************************
* 名    称：u8 Func_Plate_Motor_Shake(u8 speed,u8 time)
* 功    能：同步带电机震动时间
* 入口参数：speed ：震动速度  0 高,1中, 2低
* 入口参数：time ：震动多少时间， 单位秒
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：1A
****************************************************************************/
u8 Func_Plate_Motor_Shake(u8 speed,u8 time)
{
	u8 shake_speed = 2;
	u8 shake_time = 20;
	u8 Response_CMD[3]={0xA1,0x00,0xff};
	u8 ret = 0;

	
	shake_time = time;
	
	switch(speed)
	{
		case 0:
			shake_speed = 2;
		  shake_time  = time*13;
			break;
		case 1:
			shake_speed = 3;
		  shake_time  = time*10;
			break;
		case 2:
			shake_speed = 4;
		  shake_time  = time*8;
			break;
		default:
			break;
	}
	
	ret = Plate_Motor_Shake(shake_speed,shake_time);
	Response_CMD[1] = ret;
	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}


/****************************************************************************
* 名    称：u8 Func_ABS_Test(void)
* 功    能：ABS测试
* 入口参数：
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：1B
****************************************************************************/
u8 Func_ABS_Test(void)
{
	u8 Response_CMD[3]={0xB1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_ABS_Stop(void)
* 功    能：ABS测试
* 入口参数：
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：1C
****************************************************************************/
u8 Func_ABS_Stop(void)
{
	u8 Response_CMD[3]={0xC1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_LM_AD_Test(void)
* 功    能：光强AD测试
* 入口参数：
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：1D
****************************************************************************/
u8 Func_LM_AD_Test(void)
{
	u8 Response_CMD[3]={0xD1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_LM_AD_Stop(void)
* 功    能：光强AD测试停止
* 入口参数：
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：主机发送：1E
****************************************************************************/
u8 Func_LM_AD_Stop(void)
{
	u8 Response_CMD[3]={0xE1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Filter_Motor_Reset(void)
* 功    能：滤光轮电机复位
* 入口参数：void
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：主机发送：23
* 调用方法：
****************************************************************************/
u8 Func_Filter_Motor_Reset(u8 dir,u8 speed)
{
	u8 Response_CMD[3]={0x32,0x00,0xff};
	u8 ret =0;

	ret = Filter_Motor_Reset(dir,speed);
	Response_CMD[1]=ret;
	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Filter_Motor_Position(u8 dir,u8 speed,u8 index)
* 功    能：滤光轮电机定位
* 入口参数：void
* 出口参数：返回定位是否成功标志 0：成功 1：失败
* 说    明：8拍， 总共400拍一圈,1拍0.9度,  8个滤光片，间隔45度
* 调用方法：主机发送：24 
****************************************************************************/
u8 Func_Filter_Motor_Position(u8 dir,u8 speed,u8 index)
{
	u8 Response_CMD[3]={0x42,0x00,0xff};
	u8 ret =0;
	
	ret = Filter_Motor_Location(dir,speed,index);
	Response_CMD[1]=ret;
	USB_TxWrite(Response_CMD, 3);//发送响应

	return 0;
}


/****************************************************************************
* 名    称：u8 Func_Hardware_Test(void)
* 功    能：硬件测试函数
* 入口参数：void
* 出口参数：返回测试标志
* 说    明：这里主要通过串口输出硬件的系统时钟
* 调用方法：主机发送：80
****************************************************************************/
u8 Func_Hardware_Test(void)
{
	RCC_ClocksTypeDef  RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks); //获取各个时钟频率
	printf("SYSCLK_Frequency %d \r\n",RCC_Clocks.SYSCLK_Frequency);
	printf("HCLK_Frequency %d \r\n",RCC_Clocks.HCLK_Frequency);
	printf("PCLK1_Frequency %d \r\n",RCC_Clocks.PCLK1_Frequency);
	printf("PCLK2_Frequency %d \r\n",RCC_Clocks.PCLK2_Frequency);
	printf("ADCCLK_Frequency %d \r\n",RCC_Clocks.ADCCLK_Frequency);
	printf("---------------------------\r\n");
	return 0;
}


/****************************************************************************
* 名    称：u8 Func_CRC32_Test(void)
* 功    能：crc32测试函数
* 入口参数：void
* 出口参数：
* 说    明：
* 调用方法：主机发送：81
****************************************************************************/
u8 Func_CRC32_Test(void)
{
	u8 i = 0;
	u8 s[6] = {1,2,3,4,5,6};
// 	make_crc32_table();
	for(i=0;i<6;i++)
	{
		s[i] = G_UART_Buffer[i];
		printf("s[%d] = %d\r\n",i,s[i]);
	}
  printf("%08Xh\r\n", make_crc(s, 6));
	return 0;
}



/****************************************************************************
* 名    称：u8 Func_Beep_Test(u8 delay)
* 功    能：蜂鸣器测试
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：主机发送：82
****************************************************************************/
u8 Func_Beep_Test(u8 delay)
{
	u32 i =0;
	u32 delay_count =delay * 50;
	u8 Response_CMD[3]={0x28,0x00,0xff};
	
	for(i=0;i<5000;i++)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		delay_us(delay_count);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
    delay_us(delay_count);
	}
	
	Response_CMD[1] = delay;
	USB_TxWrite(Response_CMD, 3);//发送响应
	
	return 0;
}




/****************************************************************************
* 名    称：u8 Func_AD_Test(u8 mode,u8 wave,u8 channel,u8 filter_enable,u8 out_mode)
* 功    能：AD采集
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：主机发送：83
****************************************************************************/
u8 Func_AD_Test(u8 mode,u8 wave,u8 channel,u8 filter_enable,u8 out_mode)
{
// 	u16 adcx;
	float adcx;
	u32 temp;
	u16 temp1;
	u16 temp2;
	u8 i=0;
	u8 ret=0;
	float f1 = 0;
	u8 Response_CMD[3]={0x38,0x00,0xff};
	
	if((wave <0) || (wave >8) || (channel >7))
	{
		Response_CMD[1] = 1;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 1;
	}
	
	if(filter_enable==0)
	{
		ret = Filter_Motor_Location(1,9,wave);
		if(ret)
		{
			Response_CMD[1] = 2;
			USB_TxWrite(Response_CMD, 3);//发送响应
			return 2;
		}
	}
 	Adc_Init();		  		//ADC初始化
	delay_ms(20);
	if(mode == 0)//单独一个通道
	{
		
		adcx = Get_Adc_Average(channel,800);
		temp=adcx*3300/4096;
		
		temp1 = temp/1000;
		temp2 = temp%1000;

		if(out_mode == 0)
		{
			printf("wave=[%d]--channel=[%d]:%.4f ------%d mv-----%d.%03d mv\r\n",wave,channel,adcx,temp,temp1,temp2);
		}
		else if(out_mode == 1)
		{
			printf(":%.4f",adcx);
		}
		
		
// 		f1=(float)adcx*(3.3/4096);
// 		temp1 = (int)(f1/1);
// 		temp2 = (int)(f1*1000)%1000;
// 		printf("wave=[%d]--channel=[%d]:%d---%f mv --%d.%03d mv\r\n",wave,channel,adcx,f1,temp1,temp2);
	}
	else//连续读8通道
	{
		for(i=0;i<8;i++)
		{
			adcx = Get_Adc_Average(i,500);
			temp=adcx*3300/4096;
			
			temp1 = temp/1000;
			temp2 = temp%1000;
			
			if(out_mode == 0)
			{
				printf("wave=[%d]--channel=[%d]:%.4f ------%4d mv-----%d.%03d mv\r\n",wave,i,adcx,temp,temp1,temp2);
			}
			else if(out_mode == 1)
			{
				printf(":%.4f",adcx);
			}
		}
	}
	
	if(filter_enable==0)
	{
		ret = Filter_Motor_Reset(1,9);
		if(ret)
		{
			Response_CMD[1] = 3;
			USB_TxWrite(Response_CMD, 3);//发送响应
			return 3;
		}
	}
// 	f1=f1*AD_K[channel];
// 	temp1 = (int)(f1/10);
// 	temp2 = (int)(f1*1000)%1000;
// 	printf("AD_Value_Adjust[%d]:------%f mv-----%d.%03d mv\r\n",channel,f1,temp1,temp2);
	

// 	printf("test:---- %6.3f --\r\n",3.14);
// 	printf("AD_Average[0][0]:---- %d --\r\n",AD_Average[0][0]);
// 	printf("AD_K:-%f--%f--%f--%f--%f--%f--%f--%f--\r\n",AD_K[0],AD_K[1],AD_K[2],AD_K[3],AD_K[4],AD_K[5],AD_K[6],AD_K[7]);
// 	printf("AD_K:-%d--%d--%d--%d--%d--%d--%d--%d--\r\n",AD_K[0],AD_K[1],AD_K[2],AD_K[3],AD_K[4],AD_K[5],AD_K[6],AD_K[7]);
	
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Plate_Motor_Position_Loop(u8 mode,u8 speed)
* 功    能：同步带电机 循环定位 测试
* 入口参数：step ：运行多少步
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：电机一步1.8度，200步一圈
* 调用方法：主机发送：84
****************************************************************************/
u8 Func_Plate_Motor_Position_Loop(u8 mode,u8 speed)
{
	u8 Response_CMD[3]={0x91,0x00,0xff};
	u8 ret =0;
	
	if(speed == 0)
	{
		G_Step_Mode_Plus = 50;
	}
	else if(speed == 1)
	{
		G_Step_Mode_Plus = 100;
	}
	else if(speed == 2)
	{
		G_Step_Mode_Plus = 200;
	}

	ADC_Configuration(0);//AD通道模式初始化
	DMA_Configuration(0);
	
	ret = Filter_Plate_Motor_Loop(mode,G_Step_Mode_Plus);
	

	if(mode==0)
	{
		Process_AD_Value(0);//处理采集到的数据
	}

	Printf_AD_Value_Average();

	Response_CMD[1]=ret;

	USB_TxWrite(Response_CMD, 3);//发送响应
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Wave_Sample(u8 wave_main,u8 wave_second,u8 col,u8 mode,u8 out_mode)
* 功    能：波长测样
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：主机发送：85
****************************************************************************/
u8 Func_Wave_Sample(u8 wave_main,u8 wave_second,u8 col,u8 mode,u8 out_mode)
{
 	u8 Response_CMD[3]={0x58,0x00,0xff};
 	u8 wave_length[2]={0};
	u8 count =0;
	u8 filter_mode =0;
	u8 col_index =0;
// 	u16 *address = 0;
// 	u16 *address1 = 0;
	float *address = 0;
	float *address1 = 0;
	u8 i =0;
	u8 ret =0;
  
	
  //检测参数是否正确
	if((wave_main > 8) ||(wave_second > 8)||((wave_main==0)&&(wave_second > 0))||((wave_main==0)&&(wave_second==0)))
	{
		Response_CMD[1]=1;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 1;//参数错误
	}
	
	wave_length[0]=wave_main;
	wave_length[1]=wave_second;
  filter_mode = mode;
	col_index = col;
	
	if(wave_second)
	{
		Wavelength_Flag=1;//使用双波长测量
		count = 2;
	}
	else
	{
		Wavelength_Flag=0;//使用单波长测量
		count = 1;
	}
	
	//打开灯源
// 	G_Power_On_Off = 0;
// 	GPIO_SetBits(GPIOA,GPIO_Pin_8);	//开光源灯

	//初始化AD
	Adc_Init();		
	
	//循环两次：主波长和次波长
	for(i=0;i<count;i++)
	{
		if(filter_mode==0)
		{
			//滤光轮定位到相应的波长：先 主 后 次
			ret = Filter_Motor_Location(1,9,wave_length[i]);
			if(ret)//定位失败
			{
				Response_CMD[1]=ret;
				USB_TxWrite(Response_CMD, 3);//发送响应
				return 2;
			}
		}
		
		if (filter_mode==0)
		{
			//酶标板复位
			ret = Plate_Motor_Reset(1);
			if(ret)//复位失败
			{
				Response_CMD[1]=ret;
				USB_TxWrite(Response_CMD, 3);//发送响应
				return 3;
			}
		}
		
		if(i==0)//主波长
		{
			address  = &AD_ALLlight_Single[0];//保存主波长下的直射AD值
			address1 = &AD_Halflight_Single[0][0];//保存主波长下的12列8通道的AD值
		}
		else if(i==1)//次波长
		{
			address  = &AD_ALLlight_Double[0];//保存次波长下的直射AD值
			address1 = &AD_Halflight_Double[0][0];//保存次波长下的12列8通道的AD值
		}
		
		if(filter_mode==0)
		{
			//采集8通道光源直射AD值
			ADC0_Sample_Mode(address,2);
		}
		
		if(col_index == 13)//测 全部12 列
		{
			//开始一次12列8通道走板AD采集
			AD_Loop_Sample(address1);
		}
		else//测某一列
		{
			if(filter_mode==0)
			{
				if(col_index > 0)
				{
					ret = Plate_Motor_Position(1,col_index);
					if(ret)//酶标板定位失败
					{
						Response_CMD[1]=ret;
						USB_TxWrite(Response_CMD, 3);//发送响应
						return 4;
					}
				}
			}
			
			
			if(i==0)//主波长
			{
				if(col_index==0)//测空气
				{
					address1  = &AD_ALLlight_Single[0];//保存主波长下的直射AD值
				}
				else
				{
					address1 = &AD_Halflight_Single[col_index-1][0];//保存主波长下的col_index列8通道的AD值
				}
			}
			else if(i==1)//次波长
			{
				if(col_index==0)//测空气
				{
					address1  = &AD_ALLlight_Double[0];//保存次波长下的直射AD值
				}
				else
				{
					address1 = &AD_Halflight_Double[col_index-1][0];//保存次波长下的col_index列8通道的AD值
				}
			}
			ADC0_Sample_Mode(address1,2);//采集8通道
		}
	}
	
	//关闭光源
// 	G_Power_On_Off = 1;
// 	GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//关闭光源灯

	if(filter_mode==0)
	{
		//滤光轮复位
		ret = Filter_Motor_Reset(1,9);
		if(ret)//复位失败
		{
			Response_CMD[1]=ret;
			USB_TxWrite(Response_CMD, 3);//发送响应
			return 4;
		}
	}
	
	ret = Plate_Motor_Reset(1);
	if(ret)//复位失败
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 3;
	}

	
// 	//读取系统保存8个波长对应的 K,B,T
// 	Flash_Read_K_B_T(120);	//读K,B,T
// 	Flash_Read_Std_Target(122);	//读靶值表
	
	
	//采集完AD值之后，计算ABS
	Data_Handling(wave_main,wave_second);
	
	//将采集到的96孔数据发送出去
	USB_TxWrite(Response_CMD, 3);//发送响应
	USB_Out_Sample_Abs(Wavelength_Flag,out_mode);
	
	//清除标志
	Wavelength_Flag = 0;
	
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_AD_K_B_Calibrate(u8 mode,u8 type,u8 wave,u8 std,u8 out_mode)
* 功    能：//测试 - AD 8通道-吸光度测试-性能调整，计算8通道，4个标准物质的K,B值
* 入口参数mode：0：ABCD通道 , 1：EFGH通道   2：ABCDEFG通道
* 入口参数type：0：光路性能检验,  1：液体样本检验
* 入口参数wave：波长
* 入口参数std：靶值
* 入口参数out_mode：0：详细输出-给测试人员用,  1：只输出有效数据
* 出口参数：
* 说    明：
* 调用方法：主机发送：86
****************************************************************************/
u8 Func_AD_K_B_Calibrate(u8 mode,u8 type,u8 wave,u8 std,u8 out_mode)
{
 	u8 Response_CMD[3]={0x68,0x00,0xff};
	u8 ret =0;
  u8 wave_length = wave;//总共4种波长
	//u8 i = 0;
  SelectCalib = std;
	Flash_Read_Std_Target(123);	//读靶值表

	if((type!=0)&&(type!=1))
	{
		Response_CMD[1]=1;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 1;
	}
	
	//打开光源
	G_Power_On_Off = 0;
	GPIO_SetBits(GPIOA,GPIO_Pin_8);	//开光源灯

	//ADC初始化
	Adc_Init();	

	//滤光轮定位到指定的波长
	ret = Filter_Motor_Location(1,9,wave_length);
	if(ret)//定位失败
	{
 		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 2;
	}
	//开始循环10次走板采集电压，
	//AD_Alllight[10][8] 存放 8通道直射AD
	//AD_Halflight[10][24] 存放 24孔标准物质的AD
	ret = AD_Loop_Sample_K_B(mode,type);
	if(ret) 
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 3;
	}

	
	if(out_mode == 0)
	{
		USB_Out_K_B_Abs(1,wave_length);	//打印输出---测试人员适用	
	}
	
	if(ret == 0)
	{
		//计算10次16孔的ABS及平均值
		//ABS_Array[10][16]
		//ABS_Aver_Array[16]
		Data_Handling_Optical(mode,type);
	}
	
	if(out_mode == 0)
	{
		USB_Out_K_B_Abs(2,wave_length);//打印输出---测试人员适用
	}	
	
	
	if(type==0)//0：光路性能检验
	{
		//开始计算当前波长下的 K,B
		X_Y_Kb(wave_length,mode);
	}
	else if(type==1)//1：液体样本检验
	{
		//开始修正当前波长下的吸光度
		Data_Handling_LiquidSamples(wave_length);
	}
	
	
	if(out_mode == 0)
	{
		//通过USB端口发送
		USB_Out_K_B_Abs(3,wave_length);//打印输出---测试人员适用
	}	
	
	//组织要发送的数据
	if(type==0)//0：光路性能检验
	{
		if(out_mode == 1)
		{
			Array_Combination_Optical(wave_length,mode);
			//通过USB端口发送
			USB_Out_K_B_Abs(4,wave_length);//打印输出---只输出有效数据
		}
	}
	else if(type==1)//1：液体样本检验
	{
		if(out_mode == 1)
		{
 			//通过USB端口发送
			//此功能可以直接在测样中进行-
			USB_Out_K_B_Abs(5,wave_length);//打印输出---只输出有效数据
		}
	}
	
// 	if(type==0)
// 	{
// 		//保存计算得到的K,B,T
// 		Flash_Write_K_B_T(120);
// 	}

	//关闭光源
	G_Power_On_Off = 1;
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//关闭光源灯

	//滤光轮复位
	ret = Filter_Motor_Reset(1,9);
	if(ret)//复位失败
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 4;
	}
	return 0;
}



/****************************************************************************
* 名    称：void Func_Plate_Run_Step(u8 mode,u8 plus)
* 功    能：
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：主机发送：87
****************************************************************************/
void Func_Plate_Run_Step(u8 mode,u8 plus)
{
	u8 Response_CMD[3]={0x78,0x00,0xff};

  u16 count = 200;


	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
	
	if(mode == 0)//测试挡片 到 定位光耦的距离
	{
		//让定位片进入光耦
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//光耦挡住是1， 通过为0
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			count = count - 1;
		}
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		printf("count = %d\r\n",count);
	}
	else if(mode == 1)//测试酶标板 从 开始 到 最后的行程距离
	{
		count = 600+plus;
		while((count > 0))
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			count = count - 1;
		}
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
	}
	USB_TxWrite(Response_CMD, 3);//发送响应

}


/****************************************************************************
* 名    称：u8 Func_Device_Self_Test(void)
* 功    能：裸机自检
* 入口参数：
* 出口参数：返回值的8位分别表示：
*					：7：
*					：6：
*					：5：
*					：4：
*					：3：滤光轮复位失败
*					：2：光电池和AD模块故障
*					：1：滤光轮定位故障
*					：0：酶标板故障
* 说    明：此函数主要是在还没有连接电脑时，仪器开机自检各部件，可以初步判断仪器是否正常
* 调用方法：仪器正常情况只听到一声，    -88
****************************************************************************/
u8 Func_Device_Self_Test(void)
{
	u8 Response_CMD[3]={0x88,0x00,0xff};
	u8 ret0 = 0;
	u8 ret = 0;
	u16 i = 0;
	u16 ad_value = 0;
	
	//开灯--------------------------------------------------------
	G_Power_On_Off = 0;//灯光控制任务 中 关闭 灯源
  GPIO_SetBits(GPIOA,GPIO_Pin_8);	//开光源灯, 全亮

	//蜂鸣器名叫 1秒， 检测蜂鸣器是否正常
	for(i=0;i<2000;i++)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		delay_us(200);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
    delay_us(200);
	}
	
	//酶标板定位----------------------------------------------------
 	ret = Plate_Motor_Position(1,12);
	ret0 += ret;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//发送响应

 	delay_ms(10);
	//酶标板复位
	ret = Plate_Motor_Reset(1);
	ret0 += ret;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//发送响应
	if(ret)//复位失败，蜂鸣器叫一声
	{
		for(i=0;i<1500;i++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
	}
	delay_ms(10);
	
	ret = 0; 
	//滤光轮定位到 1号滤光片-------------------------------------------
 	ret = Filter_Motor_Location(1,9,1);
	ret0 += ret;
	
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//发送响应
	if(ret)//滤光片定位失败，蜂鸣器叫一声
	{
		for(i=0;i<1500;i++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
	}
	delay_ms(20);
	
// 	ret = 0; 
// 	//进行8通道采集空气电压，用来检测光电池和AD模块是否正常-------------
//  	Adc_Init();		  		//ADC初始化
// 	for(i=0;i<8;i++)
// 	{
// 		ad_value = Get_Adc_Average(i,10);
// 		if(ad_value<50)//光线过弱--参考660取值
// 		{
// 			ret = 1; 
// 			ret0 = ret0 | 0x04; 
// 			break;
// 		}
// 	}
// 	Response_CMD[0]=4;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 2);//发送响应
	
// 	if(ret)//电压检测失败，蜂鸣器叫一声
// 	{
// 		for(i=0;i<1500;i++)
// 		{
// 			GPIO_SetBits(GPIOD,GPIO_Pin_2);
// 			delay_us(250);
// 			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
// 			delay_us(250);
// 		}
// 	}

	delay_ms(10);
	ret = 0; 	
	//滤光轮复位-----------------------------------------------------------
  ret = Filter_Motor_Reset(1,9);
	ret0 += ret;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//发送响应
	if(ret)//复位失败，蜂鸣器叫一声
	{
		for(i=0;i<1500;i++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
	}
	delay_ms(10);
	
	Response_CMD[1] = ret0;
	USB_TxWrite(Response_CMD, 3);//发送响应
	
	//关灯------------------------------------------------------------------
  GPIO_ResetBits(GPIOA,GPIO_Pin_8);	  //关光源灯
	
	//在灯光控制任务中开启灯源,
	G_Power_On_Off = 1;//灯光控制任务 中 开启 灯源， 微亮
	G_Power_On_Off_Fre = 1;
	return ret0;
}

/****************************************************************************
* 名    称：u8 Func_Std_Target_Test(u8 mode,u8 index)
* 功    能：测试-接收标准物质的靶值
* 入口参数：
* 出口参数：
* 说    明：靶值表放在 123 号扇区
* 调用方法：-89
****************************************************************************/
u8 Func_Std_Target_Test(u8 mode,u8 index)
{
	u8 Response_CMD[3]={0x98,0x00,0xff};
	u8 i =0;
	u8 try_count = 0;
	u8 *std;
	
	
	switch(index)
	{
		case 1:
			std = (u8 *)&targetValueA[0];
			break;
		case 2:
			std = (u8 *)&targetValueB[0];
			break;
		case 3:
			std = (u8 *)&targetValueC[0];
			break;
		default:
			return 1;
			break;
	}
	
	
	if(mode==0)//写靶值
	{ 
		while(try_count < 10)
		{
			//usb缓存由 11字节的命令 加 96字节的靶值
			if(G_USB_Buffer_Size < 107)
			{
				delay_ms(100);
				try_count += 1;
			}
			else
			{
				break;
			}
		}
		
		//最多等待1000毫秒后，直接从USB缓存中取出96字节的数据，这里暂时不考虑数据是否完整。
		
		for(i=0;i<96;i++)
		{
			std[i] = G_USB_Buffer[11+i];
		}
		
		Flash_Write_Std_Target(123);
		Response_CMD[1]=0;
		USB_TxWrite(Response_CMD, 3);//发送响应
	}
	else if(mode ==1)//读靶值
	{
		Flash_Read_Std_Target(123);	//读靶值表
		
		for(i=0;i<96;i++)
		{
	// 		printf("targetValue%d[%d]=%d\r\n",index,i,std[i]);
			printf(":%d",std[i]);
		}		
	}
}

/****************************************************************************
* 名    称：u8 Func_Flash_Write_Read_Test(u8 mode,u8 wave)
* 功    能：KBT测试
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：-90
****************************************************************************/
u8 Func_Flash_Write_Read_Test(u8 mode,u8 wave)
{
	u8 Response_CMD[3]={0x09,0x00,0xff};
	u8 i=0;
	u8 j=0;
	u16 *addr=0;
	float *addr_array_k;
	float *addr_array_b;
	float *addr_array_t;
	
// 	switch(wave)
// 	{
// 		case 1:
// 			addr_array_k = (float*)&Slope_405nm[0][0];
// 			addr_array_b = (float*)&Intercept_405nm[0][0];
// 			addr_array_t = (float*)&TestValue_405nm[0][0];
// 			break;
// 		case 2:
// 			addr_array_k = (float*)&Slope_450nm[0][0];
// 			addr_array_b = (float*)&Intercept_450nm[0][0];
// 			addr_array_t = (float*)&TestValue_450nm[0][0];
// 			break;
// 		case 3:
// 			addr_array_k = (float*)&Slope_492nm[0][0];
// 			addr_array_b = (float*)&Intercept_492nm[0][0];
// 			addr_array_t = (float*)&TestValue_492nm[0][0];
// 			break;
// 		case 4:
// 			addr_array_k = (float*)&Slope_630nm[0][0];
// 			addr_array_b = (float*)&Intercept_630nm[0][0];
// 			addr_array_t = (float*)&TestValue_630nm[0][0];
// 			break;
// 		default:
// 			return 1;
// 			break;
// 	}
	
	if(mode == 0)//测试写
	{
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_k = 1.1234;
// 			addr_array_k = addr_array_k +1;
// 		}
// 		
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_b = 2.3456;
// 			addr_array_b = addr_array_b +1;
// 		}

// 		for(i=0;i<24;i++)
// 		{
// 			*addr_array_t = 3.4567;
// 			addr_array_t = addr_array_t +1;
// 		}
		
		Flash_Write_K_B_T(120);
		
// 		//保存K,B,T时， 顺便 把靶值表写入flash
// 		Flash_Write_Std_Target(122);
		
		USB_TxWrite(Response_CMD, 3);//发送响应
	}
	else if(mode ==1)//测试读
	{
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_k = 0;
// 			addr_array_k = addr_array_k +1;
// 		}
// 		
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_b = 0;
// 			addr_array_b = addr_array_b +1;
// 		}

// 		for(i=0;i<24;i++)
// 		{
// 			*addr_array_t = 0;
// 			addr_array_t = addr_array_t +1;
// 		}
		
		Flash_Read_K_B_T(120);
	
		switch(wave)
		{
			case 1:
				addr_array_k = (float*)&Slope_405nm[0][0];
				addr_array_b = (float*)&Intercept_405nm[0][0];
				addr_array_t = (float*)&TestValue_405nm[0][0];
				break;
			case 2:
				addr_array_k = (float*)&Slope_450nm[0][0];
				addr_array_b = (float*)&Intercept_450nm[0][0];
				addr_array_t = (float*)&TestValue_450nm[0][0];
				break;
			case 3:
				addr_array_k = (float*)&Slope_492nm[0][0];
				addr_array_b = (float*)&Intercept_492nm[0][0];
				addr_array_t = (float*)&TestValue_492nm[0][0];
				break;
			case 4:
				addr_array_k = (float*)&Slope_630nm[0][0];
				addr_array_b = (float*)&Intercept_630nm[0][0];
				addr_array_t = (float*)&TestValue_630nm[0][0];
				break;
			default:
				break;
		}
		
		
		//取出 K,B,T	
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
// 				printf("Slope_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_k );
				printf(":%0.4f",*addr_array_k );
				addr_array_k = addr_array_k +1;
			}
		}
			
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
// 				printf("Intercept_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_b);
				printf(":%0.4f",*addr_array_b );
				addr_array_b = addr_array_b +1;
			}
		}

		for(i=0;i<8;i++)
		{
			for(j=0;j<5;j++)
			{
// 				printf("TestValue_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_t);
				printf(":%0.4f",*addr_array_t );
				addr_array_t = addr_array_t +1;
			}
		}
	}
		
	return 0;
}

/****************************************************************************
* 名    称：u8 Func_Get_Temperature(u8 index)
* 功    能：获取温度
* 入口参数：index ：温度传感器索引
* 出口参数：返回复位是否成功标志 0：成功 1：失败
* 说    明：
* 调用方法：
****************************************************************************/
// float Func_Get_Temperature(u8 index)
// {
// 	u8 * promid = NULL;
// 	float wd=0;

// 	switch(index)
// 	{
// 		case 0:
// 			promid = DS18B20_ID01;
// 			break;
// 		case 1:
// 			promid = DS18B20_ID02;
// 			break;
// 		case 2:
// 			promid = DS18B20_ID03;
// 			break;
// 		case 3:
// 			promid = DS18B20_ID04;
// 			break;
// 		default:
// 			break;
// 	}
// 	wd = DS18B20_Get_Temp(promid);
// 	return wd;
// }

/****************************************************************************
* 名    称：u8 Func_CRC32_Cmd_Check(u8* src, u8 len)
* 功    能：crc32指令验证
* 入口参数：void
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
u8 Func_CRC32_Cmd_Check(u8* src, u8 len)
{
	u32 crc_result =0;
	
	
	//判断缓冲区数据是否接收完整，一条指令11个字节
	if(len >= 11)
	{
		if(*(src+10)!=0xff)
		{
			return 1;
		}
		return 0;
		//判断指令是否有效
		crc_result = make_crc(src,6);//生成CRC32校验码
		printf("crc_result = %d\r\n",crc_result);
		printf("%d = %d\r\n",src[6],crc_result>>24);
		printf("%d = %d\r\n",src[7],(crc_result&0x00ff0000)>>16);
		printf("%d = %d\r\n",src[8],(crc_result&0x0000ff00)>>8);
		printf("%d = %d\r\n",src[9],(crc_result&0x000000ff));
		
		if((src[6]==crc_result>>24)&&(src[7]==(crc_result&0x00ff0000)>>16)&&(src[8]==(crc_result&0x0000ff00)>>8)&&(src[9]==(crc_result&0x000000ff)))
		{
			return 0;//指令有效
		}
		else
		{
			return 1;//指令无效
		}
	}
	else
	{
			return 2;//指令不足
	}
}

