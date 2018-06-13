/****************************************************************************
* 文件名称：main.c
* 说    明: 主文件
* 功    能：里面主要任务逻辑
* 内置方法1：void main(void)	                    入口文件
* 内置方法2：void start_task(void *pdata)         开始任务
* 内置方法3：void MAIN_task(void *pdata)          主任务
* 内置方法4：
* 内置方法5：
* 内置方法6：
* 内置方法7：
* 内置方法8：
* 内置方法9：
* 内置方法10：
* 内置方法11：
****************************************************************************/

#include "pbdata.h"
#include "uart.h"
#include "ad.h"
#include "crc32.h"
#include "func.h"
#include "usb_config.h"
#include "ds18b20.h"
#include "motor.h"
#include "heat.h"
#include "flash.h"

//ucosII任务堆栈设置
//设置任务优先级： 优先级很重要，需要及时处理的任务必须要设置高优先级
#define START_TASK_PRIO 	12
#define TEMP_TASK_PRIO   	9
#define BEEP_TASK_PRIO   	8
#define LAMP_TASK_PRIO 	  7
#define AD_TASK_PRIO 		  6
#define FUNC_TASK_PRIO 		5
#define UART_TASK_PRIO 	  4
#define USB_TASK_PRIO 		3


//设置任务堆栈大小
#define START_STK_SIZE   128 // 空间大小=64*4(字节)
#define AD_STK_SIZE      128 // 空间大小=64*4(字节)
#define UART_STK_SIZE    128 // 空间大小=64*4(字节)
#define USB_STK_SIZE  	 512 // 空间大小=64*4(字节)
#define FUNC_STK_SIZE  	 512 // 空间大小=64*4(字节)
#define TEMP_STK_SIZE  	 128 // 空间大小=64*4(字节)
#define BEEP_STK_SIZE  	 128 // 空间大小=64*4(字节)
#define LAMP_STK_SIZE  	 128 // 空间大小=64*4(字节)

//创建任务堆栈空间
//使用 Microlib 库后，printf 等函数对浮点数的处理出现非预期值， 这时候必须在使用的堆栈前8字节对齐__align(8)
__align(8) OS_STK START_TASK_STK[START_STK_SIZE];
__align(8) OS_STK AD_TASK_STK[AD_STK_SIZE];
__align(8) OS_STK UART_TASK_STK[UART_STK_SIZE];
__align(8) OS_STK USB_TASK_STK[USB_STK_SIZE];
__align(8) OS_STK FUNC_TASK_STK[FUNC_STK_SIZE];
__align(8) OS_STK TEMP_TASK_STK[TEMP_STK_SIZE];
__align(8) OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
__align(8) OS_STK LAMP_TASK_STK[LAMP_STK_SIZE];


//任务声明
void start_task(void *pdata);
void USB_task(void *pdata);
void UART_task(void *pdata);
void FUNC_task(void *pdata);
void Ad_task(void *pdata);
void BEEP_task(void *pdata);
void TEMP_task(void *pdata);
void LAMP_task3(void *pdata);



/****************************************************************************
* 名    称：void TEMP_task(void)
* 功    能：TEMP_task功能任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明：温育任务
* 调用方法:
****************************************************************************/
void TEMP_task(void *pdata)
{
	u8 Response_CMD[3]={0x21,0x00,0xff};
	u16 temp;
	float wd;
	u8 count= 0;
	
	while(1)
	{
		count += 1; 
		//向上位机发送当前DS18B20 检测到的温度
		if(G_Temperature_Disp == 1 && count > 5)
		{
			count = 0;
			temp=DS18B20_Get_wd(1);//加热板内的温度
			if((temp&0xF800)==0xF800)//负温度判断
			{
				temp=~temp;
				temp=temp+1;
				wd=temp*(-0.0625);
			}
			else
			{
				wd=temp*0.0625;	
			}
			if (wd < 80)
			{
				printf(":%.1f:",wd);
			}
// 			printf("%d %f",Target_Temperature ,wd);
			delay_ms(1000);
		}
		
		if(G_Heat3_Start == 1)   //加热片加热
		{
			//G_Beep_on_off = 1;
			Func_Temperature_Heat(Target_Temperature);  //该函数需要一个参数
			//G_Beep_on_off = 0;
			delay_ms(1000);
		}
		else if(G_Heat3_Start == 2)//待机-不工作
		{
			Heat3_Control(Target_Temperature,IDLESTATE);
			delay_ms(1000);
		}

    if(G_Heat1_Start==1)    //帕尔帖加热
		{
			//G_Beep_on_off = 1;			
			Func_Peltier_Board_Heat(Target_Temperature);
			//G_Beep_on_off = 0;
			delay_ms(1000);
		}
		else if(G_Heat1_Start == 2)//待机-不工作
		{
			Heat1_Control(Target_Temperature,IDLESTATE);
			delay_ms(1000);
		}
		
		if(G_Heat2_Start==1)    //帕尔帖制冷
		{
			//G_Beep_on_off = 1;			
			Func_Peltier_Board_Cold(Target_Temperature);
			//G_Beep_on_off = 0;
			delay_ms(1000);
		}
		else if(G_Heat2_Start == 2)//待机-不工作
		{
			Heat2_Control(Target_Temperature,IDLESTATE);
			delay_ms(1000);
		}
		
		
		delay_ms(500);
	}
}

/****************************************************************************
* 名    称：void LAMP_task(void)
* 功    能：控制卤素灯待机亮度功能任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明：蜂蜜器任务
* 调用方法:
****************************************************************************/
void LAMP_task(void *pdata)
{
	while(1)
	{
		if(G_Power_On_Off)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_8);	//开光源灯
			delay_ms(2*G_Power_On_Off_Fre);
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//关闭光源灯
			delay_ms(1);
		}
		delay_ms(10);
	}
}

/****************************************************************************
* 名    称：void BEEP_task(void)
* 功    能：BEEP_task功能任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明：蜂蜜器任务
* 调用方法:
****************************************************************************/
void BEEP_task(void *pdata)
{
// 	uint8_t err;
	uint8_t i=0;
	
	while(1)
	{
//     printf("---BEEP_task02---\r\n");
		if(G_Beep_on_off)
		{
			for(i=0;i<50;i++)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
	// 			delay_us(100*beep_count);
				delay_us(100);
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);
				delay_us(100);
	// 			delay_us(100*beep_count);
			}
			delay_ms(10);
		}
		else
		{
			delay_ms(20);
		}

	}
}

/****************************************************************************
* 名    称：void Ad_task(void)
* 功    能：AD测试任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明：单通道， 连续转换模式 ， DMA非扫描
* 调用方法：无 
****************************************************************************/
void Ad_task(void *pdata)
{
//   uint32_t len = 0;
// 	u8 error = 0;
//   uint8_t buf[200] = {0};
	u8 i=0;
	pdata=pdata;

	while(1)
	{		
// 		printf("---Ad_task---\r\n");

		if (AD_MODE ==0)
		{
			if(i==12)//总共12列
			{
				Process_AD_Value(AD_MODE);//处理采集到的数据
				Printf_AD_Value_Average();
// 				Printf_AD_Value(AD_MODE);
				Clear_AD_Value(AD_MODE);
				delay_ms(20);
				i=0;
				AD_MODE = 3;
			}			
			AD_Sample_Mode1(i);
			i++;
		}
		else if(AD_MODE ==1)
		{
			if(i==12)//总共12列
			{
				Process_AD_Value(AD_MODE);//处理采集到的数据
				Printf_AD_Value_Average();
// 				Printf_AD_Value(AD_MODE);
				Clear_AD_Value(AD_MODE);
				delay_ms(20);
				i=0;
				AD_MODE = 3;
			}
			AD_Sample_Mode2(i);
			i++;
		}
		else
		{
			delay_ms(20);
		}
	}
}

/****************************************************************************
* 名    称：void FUNC_task(void)
* 功    能：FUNC_task功能任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明：此任务被动响应各种控制命令动作
* 调用方法:
****************************************************************************/
void FUNC_task(void *pdata)
{
	u8 Response_CMD[3]={0x00,0x00,0xff};
	uint8_t err;
	uint8_t i=0;
	u16 temp;
	OS_CPU_SR cpu_sr=0;
	
	while(1)
	{
//printf("---FUNC_task---\r\n");

		//等待指令信号的到来
		G_Func_Running = 0;
		delay_ms(10);
		OSSemPend(Func_Sem,0,&err);
		//接收到信号
		G_Func_Running = 1;
		
		switch(G_Func_Type)
		{
			case FUNC_TYPE_CONNECT: //联机检测  03
				G_Beep_on_off = 1;
				Func_Front_Connect_Test();
				delay_ms(5);				
				G_Beep_on_off = 0;
				break;
			case FUNC_TYPE_SELF_TEST: //开机自检 04
				G_Beep_on_off = 1;
				Func_Front_Self_Test();
				delay_ms(5);				
				G_Beep_on_off = 0;
				break;
			case FUNC_TYPE_PREPARE_TEST: //准备测试
				Func_Sample_Test_Prepare();
				break;
			case FUNC_TYPE_START_TEST: //开始测试
				Func_Sample_Test_Start();
				break;
			case FUNC_TYPE_STOP_TEST: //测试停止
				Func_Sample_Test_Stop();
				break;
			case FUNC_TYPE_COM_OPEN: //打开串口
				Func_Com_On();
				break;
			case FUNC_TYPE_COM_CLOSE: //关闭串口
				Func_Com_Off();
				break;
			case FUNC_TYPE_USB_OPEN: //打开USB端口
				Func_USB_On();
				break;
			case FUNC_TYPE_USB_CLOSE: //关闭USB端口
				Func_USB_Off();
				break;
			case FUNC_TYPE_LIGHT_ON: //打开光源    0D
				G_Power_On_Off = 0;
			  Func_Light_On(); //全亮
				break; 
			case FUNC_TYPE_LIGHT_OFF: //关闭光源   0E
				//G_USB_Buffer[1]： 灯光亮度 频率 最小值 1
				Func_Light_Off(G_USB_Buffer[1]);
				break;
///////////////////////////////////////////////////////////////////				
			case FUNC_TYPE_TEMPERATURE_READ: //温控 12
				if (G_USB_Buffer[1]==0)
				{
					G_Temperature_Disp = 1;//发送当前温度数据
				}
				else if (G_USB_Buffer[1]==1)
				{
					G_Temperature_Disp = 0;//不发送当前温度数据
				}
				else if (G_USB_Buffer[1]==2)
				{
					//加热状态 --同时发送当前温度数据
					G_Temperature_Disp = 1;
					Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //目标温度计算
					Func_Temperature_Hot(Target_Temperature);  

				}
				else if (G_USB_Buffer[1]==3)
				{
					//制冷状态--同时发送当前温度数据
					G_Temperature_Disp = 1;
					Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //目标温度计算
					Func_Temperature_Cold(Target_Temperature);  
				}
				else if(G_USB_Buffer[1]==4)
				{
					//待机状态--加热片，帕贴不在工作，定时器没有关闭，输出PWM脉冲，即占空比为0--此时加热片-帕贴都不工作
					G_Temperature_Disp = 0;//
					Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //目标温度计算
					Func_Temperature_Idle(Target_Temperature);
				}
				else if(G_USB_Buffer[1]==5)
				{
					//关闭状态--加热片，帕贴不在工作，定时器也不在产生PWM脉冲
					G_Temperature_Disp = 0;//
					Func_Temperature_Off();
				}
				break;
			case FUNC_TYPE_TEMPERATURE_ON: //加热片开启 13
				Heat3_Init();
			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //目标温度计算
				G_Heat3_Start =1;//开始加热
			  G_Temperature_Disp = 2;//加热片发送温度数据
				break;
			case FUNC_TYPE_TEMPERATURE_OFF: //加热片关闭  14
				//加热片 和帕贴 如果同时在工作， 二者都用到了定时器， 所以关闭加热片的时候不能关闭定时器，只能让另一个处于待机状态
				if(G_Heat1_Start !=1 && G_Heat2_Start!=1)
				{
					G_Temperature_Disp = 0;
					Func_Temperature_Off();
				}
				else
				{
					G_Temperature_Disp = 3;
					G_Heat3_Start =2;//关闭加热
				}
				Response_CMD[0]=0x41;
				Response_CMD[1]=0x00;
			  G_Temperature_Disp = 0;
				USB_TxWrite(Response_CMD, 3);//发送响应
				break;
			case FUNC_TYPE_TEMPERATURE_HOT: //帕贴启动  15
				
			  G_Temperature_Disp = 3;//帕贴发送温度数据
				//先读取帕贴模块的温度
				i=0;
				while(i<100)
				{
					temp=DS18B20_Get_wd(2);
					temp = Temperature_convert(temp);
					if (temp >10 && temp < 700)//第一次读温度经常出现850
					{
						break;
					}
					delay_ms(10);
				}
				
			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //目标温度计算
// 				printf("%d - %d\r\n",temp,Target_Temperature);
				if(temp < Target_Temperature)//说明设定的温度高于环境温度--加热
				{
					Peltier_Init(1);
					G_Heat1_Start =1;//开始加热
					G_Heat2_Start =0; 
				}
				else if(temp > Target_Temperature)//说明设定的温度小于环境温度--制冷
				{
					Peltier_Init(3);
					G_Heat2_Start =1;              //开始制冷
					G_Heat1_Start =0;
				}
				
				break;
			case FUNC_TYPE_TEMPERATURE_COLD: //帕贴关闭 16
				if(G_Heat3_Start != 1)
				{
					G_Temperature_Disp = 0;
					Func_Temperature_Off();
				}
				else if(G_Heat1_Start == 1)
				{
					G_Temperature_Disp = 2;
					G_Heat1_Start = 2;
				}				
				else if(G_Heat2_Start == 1)
				{
					G_Temperature_Disp = 2;
					G_Heat2_Start =2; 
				}
				Response_CMD[0]=0x61;
				Response_CMD[1]=0x00;
				USB_TxWrite(Response_CMD, 3);//发送响应
				break;
// 			case FUNC_TYPE_TEMPERATURE_HOT: //帕贴加热   15
// 				
// 			  Peltier_Init(1);
// 			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //目标温度计算
// 			  G_Heat1_Start =1;//开始加热
// 				break;
// 			case FUNC_TYPE_TEMPERATURE_COLD: //帕贴制冷  16
// 				Peltier_Init(3);
// 			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //目标温度计算
// 			  G_Heat2_Start =1;              //开始制冷
// 				break;
			
///////////////////////////////////////////////////////////////////			
			case FUNC_TYPE_PLATE_RESET: //酶标板复位  17
				//G_USB_Buffer[1]： 速度    参考值 1
 				Func_Plate_Motor_Reset(G_USB_Buffer[1]);
				break;
			case FUNC_TYPE_PLATE_RUN: //酶标板运行,用来测试酶标板的参数   18
				//G_USB_Buffer[1]： 速度    参考值 10
				//G_USB_Buffer[2]： 步数    
				Func_Plate_Motor_Run(G_USB_Buffer[1],G_USB_Buffer[2]);
				delay_ms(500);
				break;
			case FUNC_TYPE_PLATE_POSITION: //酶标板定位   19
				//G_USB_Buffer[1]： 速度
				//G_USB_Buffer[2]： 索引列
  			Func_Plate_Motor_Position(G_USB_Buffer[1],G_USB_Buffer[2]);
				break;
			case FUNC_TYPE_PLATE_SHAKE: //酶标板震板   1A
				//G_USB_Buffer[1]： 振板强度
				//G_USB_Buffer[2]： 振板时间
				Func_Plate_Motor_Shake(G_USB_Buffer[1],G_USB_Buffer[2]);
				delay_ms(500);
				break;
			case FUNC_TYPE_ABS_TEST: //ABS测试    1B
				Func_ABS_Test();
				AD_MODE = 0;
 				ADC_Configuration(AD_MODE);//AD通道模式初始化
				DMA_Configuration(AD_MODE);
				break;
			case FUNC_TYPE_ABS_STOP: //ABS停止    1C
				Func_ABS_Stop();
				AD_MODE = 3;
				break;
			case FUNC_TYPE_LUMINOUS_TEST: //光强AD测试  1D
				Func_LM_AD_Test();
				AD_MODE = 1;
 				ADC_Configuration(AD_MODE);//AD通道模式初始化
				DMA_Configuration(AD_MODE);
				break;
			case FUNC_TYPE_LUMINOUS_STOP: //光强AD停止  1E
				Func_LM_AD_Stop();
				AD_MODE = 3;
				break;
			case FUNC_TYPE_FILTER_RESET: //滤光轮复位    23
				//G_USB_Buffer[1]： 方向
				//G_USB_Buffer[2]： 速度  参考值 8
				Func_Filter_Motor_Reset(G_USB_Buffer[1],G_USB_Buffer[2]);
				delay_ms(500);
				break;
			case FUNC_TYPE_FILTER_POSITION: //滤光轮定位  24
				//G_USB_Buffer[1]： 方向
				//G_USB_Buffer[2]： 速度   参考值 9
				//G_USB_Buffer[3]： 索引，可以放8个滤光片的序号 0-7
				Func_Filter_Motor_Position(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3]);
			  delay_ms(500);
				break;
			case 0x80://测试 - 硬件时钟
 				Func_Hardware_Test();
				break;
			case 0x81://测试 - crc32校验值
 				Func_CRC32_Test();
				break;
			case 0x82://测试 - 蜂鸣器
				//G_USB_Buffer[1]： 蜂鸣器延时
				OS_ENTER_CRITICAL();//进入临界区，中断无法打断
 				Func_Beep_Test(G_USB_Buffer[1]);
				OS_EXIT_CRITICAL()//退出临界区 可以被中断打断
				break;
			case 0x83://测试 - AD 各个通道
				G_Temperature_Disp = 0;//测样的时候，不要发送温度数据
 				//G_USB_Buffer[1]： 0：读单通道 ，1：连续读8通道
 				//G_USB_Buffer[2]： 第几波长
 				//G_USB_Buffer[3]： 第几通道 ,读单通道才需要此参数
 				//G_USB_Buffer[4]： 是否自动开启滤光片定位 ：0：开启  1：不开启
 				//G_USB_Buffer[5]： 是详细输出结果 还是 简单输出结果
				Func_AD_Test(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3],G_USB_Buffer[4],G_USB_Buffer[5]);
				break;
			case 0x84://测试 - 酶标板步进模式
 				//G_USB_Buffer[1]：进板模式：步进
 				//G_USB_Buffer[2]：进板速度： 高，中，低
				Func_Plate_Motor_Position_Loop(G_USB_Buffer[1],G_USB_Buffer[2]);
				break;
			case 0x85://测试 - 双波长测样
				G_Temperature_Disp = 0;//测样的时候，不要发送温度数据
				//G_USB_Buffer[1]： 主波长 
 				//G_USB_Buffer[2]： 次波长
 				//G_USB_Buffer[3]： 第几列， 0：无，1：第一列 ... 13:所有12列
 				//G_USB_Buffer[4]： 是否开启滤光轮自动定位 0：开启 ，1：不开启
 				//G_USB_Buffer[5]： 是否详细输出测试数据 0：详细输出 ，1：只输出结果
				Func_Wave_Sample(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3],G_USB_Buffer[4],G_USB_Buffer[5]);
				break;
			case 0x86://测试 - AD 8通道-吸光度测试-性能调整，计算8通道，6个标准物质的K,B值
				G_Temperature_Disp = 0;//测样的时候，不要发送温度数据
 				//G_USB_Buffer[1]： 0：ABCD通道 ,     1：EFGH通道  
 				//G_USB_Buffer[2]： 0：光路性能检验,  1：液体样本检验 (此时不需要第一个参数G_USB_Buffer[1])
 				//G_USB_Buffer[3]： 波长
 				//G_USB_Buffer[4]： 使用哪个标准物质的靶值
 				//G_USB_Buffer[5]： 0：详细输出测试数据,  1：只输出有效数据
				Func_AD_K_B_Calibrate(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3],G_USB_Buffer[4],G_USB_Buffer[5]);
				break;
			case 0x87://测试酶标板运行的行程
				//G_USB_Buffer[1] 0：测试酶标板到 定位光耦的行程， 1：测试整个行程
				//G_USB_Buffer[2]
				Func_Plate_Run_Step(G_USB_Buffer[1],G_USB_Buffer[2]);
				break;
			case 0x88://测试 - 酶标板自检
				Func_Device_Self_Test();
				break;
			case 0x89://测试 - 接收标准物质的靶值
				G_Temperature_Disp = 0;//测样的时候，不要发送温度数据
				//G_USB_Buffer[1] :表示 读写  0：写flash， 1：读flash   
				//G_USB_Buffer[2] :索引
				Func_Std_Target_Test(G_USB_Buffer[1],G_USB_Buffer[2]);				
				break;
			case 0x90://测试 - K、B、T值的读写
				G_Temperature_Disp = 0;//测样的时候，不要发送温度数据
				//G_USB_Buffer[1] :表示 读写  0：写flash， 1：读flash   
				//G_USB_Buffer[2] :波长
				Func_Flash_Write_Read_Test(G_USB_Buffer[1],G_USB_Buffer[2]);				
				break;
			default:
				break;
		}
		
// 		USB_TxWrite(G_USB_Buffer, G_USB_Buffer_Size);//测试接收到的数据
		// 清空USB指令缓存
		//USB接口一旦收到11字节的指令，就会触发相应的功能执行，此时USB缓存里面开头存放的是11字节指令，后面存放的是上位机后续发送的指令数据。
		//当前指令执行完毕后，即使在此期间又收到了指令，数据，一律不处理。即清空USB缓存，重新开始接收数据。
		//所以要想每条命令都能得到处理，一定要等上条命令处理完毕为止。
 		USB_Buffer_CMD_Clear();//清除指令缓冲区
	}
}


/****************************************************************************
* 名    称：void UART_task(void)
* 功    能：串口接收数据任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明： 
* 调用方法:此任务主要接收串口发送过来的数据， 主要是条码枪
****************************************************************************/
void UART_task(void *pdata)
{
	u8 error = 0;
// 	u8 i = 0;
	u8 Response_CMD[3]={0x30,0x00,0xee};

	pdata=pdata;
	while(1)
	{
//  printf("---UART_task---\r\n");
		
		if(G_UART_Buffer_Size>0)//表明串口收到数据
		{
			if(G_Computer_Connect_Flag==0)//表明还没有连上 上位机 ， 此时串口收到的数据 可能是 上位机发送的检测串口是否存在的指令
			{
				//判断收到的指令是否正确
				error = Func_CRC32_Cmd_Check(G_UART_Buffer,G_UART_Buffer_Size);
				if(error == 2)//指令不足
				{	
					delay_ms(20);
					continue;
				}
				else if(error ==1)//指令无效
				{
					UART_Buffer_CMD_Clear();//清除指令缓冲区
					continue;
				}
				else if(error ==0)//指令有效，保存当前指令
				{
					if(G_UART_Buffer[0] == 0x03)//联机指令，测试端口存不存在
					{
						//3.指令处理
						UART_Buffer_CMD_Send1(Response_CMD,2);
						UART_Buffer_CMD_Clear();//清除指令缓冲区
						delay_ms(20);
					}
				}
			}
			else if(G_Computer_Connect_Flag==1)//表明已经和上位机通讯连接成功
			{
				
				
				UART_Buffer_CMD_Send1(G_UART_Buffer,G_UART_Buffer_Size);
				UART_Buffer_CMD_Clear();//清除指令缓冲区
				
				delay_ms(20);
			}	
		}
		else
		{
			delay_ms(20);
		}
	}
}


/****************************************************************************
* 名    称：void USB_task(void)
* 功    能：USB 任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明：此任务接收串口中断产生的命令，为了能快速响应主机发送过来的命令，此任务优先级要比其它任务要高，
*           否则其它高优先级任务会阻塞此任务，导致主机发送过来的命令得不到及时响应，此时表现主机不能控制仪器的故障
*           其次：上位机不要连续发送命令，否则下位机只能识别处理第一条命令，后面发的命令将不处理直接舍弃
* 调用方法：此任务负责 USB 端口的接收， USB端口接收控制命令
****************************************************************************/
void USB_task(void *pdata)
{
  uint32_t len = 0;
	u8 error = 0;
  uint8_t buf[100] = {0};

	//DS18B20初始化
  error = DS18B20_Configuration(1);
// 	printf("error1 = %d\r\n",error);
	G_Error_Type |= error<<0;
  error = DS18B20_Configuration(2);
// 	printf("error2 = %d\r\n",error);
	G_Error_Type |= error<<1;
	
  //裸机自检
	Func_Device_Self_Test();
	
  //读取系统保存8个波长对应的 K,B,T
	Flash_Read_K_B_T(120);	//读K,B,T

	//注意：仪器烧录程序后，里面保存的校正因子 和 靶值表 都是 无效的，
  //1.必须先确定靶值表， 软件内部会有一个默认的靶值表， 如果要使用新的靶值表，还必须将新的靶值表保存到仪器的flash中。
  //2.必须先进行光性能校正后写入正确的校正因子


	while(1)
	{	
		//从USB串口接收数据
		len = USB_RxRead(buf, sizeof(buf));
		if (len > 0)
		{
			//保存到usb串口缓存
			Copy_To_Usb_Buf(buf,len,G_USB_Buffer);
			
			if(G_Func_Running==1)//FUNC任务函数正在执行，说明之前的命令还没有执行完毕，如果有数据到来就直接保存在缓存区
			{
				delay_ms(20);
				continue;
			}

			//检测收到的指令数据是否正确
			error = Func_CRC32_Cmd_Check(G_USB_Buffer,G_USB_Buffer_Size);
			if(error == 2)//指令不足
			{
				delay_ms(20);
				continue;
			}
			else if(error == 1)//指令错误
			{
				USB_Buffer_CMD_Clear();//清除USB缓存
				continue;
			}
			else if(error ==0)//指令有效
			{	
// 				//已经收到11个数据，如果此时只是单纯的命令，后面不跟数据,即把超过命令的部分读完并舍弃
// 				if(G_USB_Buffer[0]!=0x89)//0x89 接收靶值命令，后面会有靶值表数据
// 				{
// 					do
// 					{
// 						//把USB缓存中的数据读完，这里主要是防止，用户连续发送了多个命令，只把第一个命令作为有效，后面的命令全舍弃，保证每次单独处理一条指令
// 						len = USB_RxRead(buf, sizeof(buf));
// 					}while(len>0);
// 				}

				//更新指令，记录前一次指令和当前指令，这里记录前后两条指令，主要用来为以后做准备，处理前后指令相关的功能。
				if((G_Pre_Cmd_Type == G_Curr_Cmd_Type))//前后两条指令相同
				{
					G_Curr_Cmd_Type = G_USB_Buffer[0];//更新当前指令
				}
				else//前后两条指令不相同
				{
					G_Pre_Cmd_Type  = G_Curr_Cmd_Type;// 更新指令
					G_Curr_Cmd_Type = G_USB_Buffer[0];// 更新指令
				}
				
				//指令处理
				G_Func_Type = G_Curr_Cmd_Type;
// 				USB_TxWrite(G_USB_Buffer, G_USB_Buffer_Size);//测试接收到的命令
				if(G_Func_Running==0)//FUNC任务函数空闲
				{
					OSSemPost(Func_Sem);
				}				
			}
		}
		else
		{
			delay_ms(20);
		}
	}
}


/****************************************************************************
* 名    称：void start_task(void)
* 功    能：开始任务
* 入口参数：void *pdata
* 出口参数：无
* 说    明：此任务是系统的第一个任务，只负责创建其它任务的工作。
* 调用方法：无 
****************************************************************************/
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;

	pdata=pdata;
	
	Func_Sem = OSSemCreate(0);

	OSStatInit();//初始化统计任务

	OS_ENTER_CRITICAL();//进入临界区，中断无法打断
	OSTaskCreate(USB_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&USB_TASK_STK[USB_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								USB_TASK_PRIO);//分配给任务的优先级
	
	OSTaskCreate(UART_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&UART_TASK_STK[UART_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								UART_TASK_PRIO);//分配给任务的优先级
								
	OSTaskCreate(Ad_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&AD_TASK_STK[AD_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								AD_TASK_PRIO);//分配给任务的优先级

	OSTaskCreate(FUNC_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&FUNC_TASK_STK[FUNC_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								FUNC_TASK_PRIO);//分配给任务的优先级
								
	OSTaskCreate(TEMP_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&TEMP_TASK_STK[TEMP_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								TEMP_TASK_PRIO);//分配给任务的优先级
								
	OSTaskCreate(BEEP_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&BEEP_TASK_STK[BEEP_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								BEEP_TASK_PRIO);//分配给任务的优先级
								
	OSTaskCreate(LAMP_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&LAMP_TASK_STK[LAMP_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								LAMP_TASK_PRIO);//分配给任务的优先级

 	OSTaskSuspend(START_TASK_PRIO);//挂起开始任务
	OS_EXIT_CRITICAL()//退出临界区 可以被中断打断
	
}


/****************************************************************************
* 名    称：int main(void)
* 功    能：主函数入口
* 入口参数：无
* 出口参数：无
* 说    明：在main函数之前就已经调用SystemInit()函数，各系统时钟配置后如下：
*           SysClk =72MHZ  ， =PLLCLK  ,=外部晶振(8MHZ) * PLL倍频系数PLLMUL=9
*						AHBCLK =72MHZ  ， =SysClk/AHB   , 对应 HCLK
*						APB1CLK=36MHZ  ， =AHBCLK/APB1  , 对应 PCLK1
*						APB2CLK=72MHZ  ， =AHBCLK/APB2  , 对应 PCLK2
*					  系统定时器SysTick时钟频率 = 9MHZ , =AHBCLK/8
* 调用方法：无 
****************************************************************************/
int main(void)
{
	//1.时钟初始化
	RCC_Configuration();//系统时钟初始化
	//2.端口GPIO初始化
	GPIO_Configuration();//端口初始化
	//3.系统滴答时钟初始化
 	Delay_Init();//ucosii时系统定时器初始化
// 	NVIC_Configuration(NVIC_PriorityGroup_2,SysTick_IRQn,0,0); //SysTick，中断分组2，抢占优先级0，响应优先级0
	//4.串口1初始化
  USART_Configuration(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No);//串口1：波特率9600， 8个数据位，1个停止位，无校验位
  NVIC_Configuration(NVIC_PriorityGroup_2,USART1_IRQn,1,1); //串口1，中断分组2，抢占优先级1，响应优先级1
	
	//Pwm_Init();
	
	//AD1初始化
// 	ADC_Configuration(AD_MODE);//AD单通道模式初始化
//  	Adc_Init();		  		//ADC初始化
	
	//DMA初始化
	DMA_Configuration(AD_MODE);
	
	//USB初始化
 	USB_Configuration();//USB初始化
	
	//DS18B20初始化
//   DS18B20_Configuration();

	//生成CRC32	校验码表
	make_crc32_table();

	//UCOS初始化
	OSInit();//初始化UCOS操作系统
	//创建开始任务
	OSTaskCreate(start_task,//指向任务代码的指针
								(void *)0,//任务开始执行时，传递给任务参数的指针
								(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],//分配给任务堆栈的栈顶指针
								START_TASK_PRIO);//分配给任务的优先级
	//启动ucos
	OSStart();//启动ucos操作系统
}


