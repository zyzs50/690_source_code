/****************************************************************************
* 文件名称：motor.c
* 说    明：电机文件
* 功    能：里面主要放同步带电机 和 滤光轮电机动作函数
* 内置方法1：uint8_t Plate_Motor_Reset(void)  // 同步带电机复位
* 内置方法2：uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse)//同步带电机运动多少个脉冲
* 内置方法3：
* 内置方法4：
* 内置方法5：
* 内置方法6：uint8_t Filter_Motor_Location(uint8_t dir,uint8_t position) //滤光轮电机定位
* 内置方法7：uint8_t Filter_Motor_Reset(void) //滤光轮电机复位
* 内置方法8：
* 内置方法9：
* 内置方法10：
* 内置方法11：
****************************************************************************/
#include "motor.h"
#include "ad.h"

int8_t step_index = 0;//滤光轮4项8拍的索引
int8_t plate_motor_stop_flag = 0;//同步带电机停止标志
int8_t filter_motor_stop_flag = 0;//同步带电机停止标志



//----------------------------------------------下面是同步带电机相关函数---------------------------------------------------------------//

/****************************************************************************
* 名    称：uint8_t Plate_Motor_Reset(u8 speed)
* 功    能：同步带电机复位
* 入口参数1：无
* 出口参数：0:复位成功，
* 出口参数：1:复位失败，同步带定位光耦故障
* 出口参数：2:复位失败，霍尔传感器硬件故障
* 出口参数：3:复位失败，硬件故障
* 说    明：定位挡片到光耦的行程48步
* 调用方法：注意酶标板复位 可以只需要用到霍尔传感器就可以了，这里写的稍微复杂点，把定位光耦也考虑进来，
*           挡片离开定位光耦到达复位点有一定的脉冲间隔，所以根据这点关系，更好的控制复位。主要是想尽量提高系统的稳定性。
*           此种方法 可以 避免 及时 霍尔传感器故障，酶标板复位的时候不会一直运动，此时可以借助定位光耦来停止酶标板继续运动
****************************************************************************/
uint8_t Plate_Motor_Reset(u8 speed)
{
	uint8_t speed_delay = 0;
	uint16_t pluse_count = 0;
	uint16_t position_opt_count = 0;//定位光耦脉冲监测计数
	
	speed_delay = speed;
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机

	//首先检查是否已经位于复位点
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0)//读霍尔传感器点  更改为Pin_5
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		return 0;//已经处于复位点，返回0
	}
	
	//不在复位点，此时会存在两种情况，位于复位点的左右两侧
	//所以可以先让同步带电机进仓方向运行60个脉冲并检测，检测不到然后再反向运行判断
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
	pluse_count = 100;//挡片 进入定位光耦槽 行程为 52个脉冲
	//循环退出的条件：检测到复位点， 定位挡片已经进入光耦处，脉冲走完
// 	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(pluse_count > 0))
	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(pluse_count > 0))  //更改为Pin_5
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 		delay_ms(1);
		delay_ms(speed_delay);
		GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 		delay_ms(1);
		delay_ms(speed_delay);
		pluse_count = pluse_count - 1;
	}
	delay_ms(20);

	//判断是否已经位于复位点
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0)//读霍尔传感器点 , 更改为Pin_5
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		return 0;//已经处于复位点，返回0
	}
	
	if(pluse_count == 0)//同步带定位光耦故障
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		G_Error_Type |= 1<<2; //记录异常状态
		return 1;
	}
	
	//挡片已经进入定位光耦
	if ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0))//定位挡片已经进入光耦处
	{
		//没找到复位点，反向运动
		GPIO_SetBits(GPIOB,GPIO_Pin_1);//出仓方向
		pluse_count = 605;
// 		while((pluse_count > 0))
 		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != 0)&&(pluse_count > 0))  //读霍尔传感器点 , 更改为Pin_5
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	// 		delay_ms(1);
			delay_ms(speed_delay);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
	// 		delay_ms(1);
			delay_ms(speed_delay);
			pluse_count = pluse_count - 1;
			
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)//判断挡片是否已经离开定位光耦
			{
				position_opt_count =position_opt_count +1;
				if(position_opt_count > 50)
				{
					break;
				}
			}
			else
			{
				position_opt_count = 0;
			}
		}
		
		//挡片离开定位光耦，而且脉冲数达到48， 此时很可能是霍尔传感器硬件故障
		if(position_opt_count > 50)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			G_Error_Type |= 1<<3; //记录异常状态
			return 2;//复位失败
		}
		
		//605个脉冲走完，都没找到，肯定硬件故障,此时同步带电机故障的可能性比较大
		if(pluse_count==0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			G_Error_Type |= 1<<4; //记录异常状态
			return 3;//复位失败
		}
		else
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			G_Error_Type &= ~((1<<2)|(1<<3)|(1<<4)); //清除异常状态
			return 0;//复位成功
		}
	}
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
	G_Error_Type &= ~((1<<2)|(1<<3)|(1<<4)); //清除异常状态
	return 0;//复位成功
}

/****************************************************************************
* 名    称：uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse)
* 功    能：同步带电机运行指定的脉冲数
* 入口参数1：方向 
* 入口参数1：速度    
* 入口参数1：脉冲数  
* 出口参数：0: 成功，运行了指定的脉冲数
* 出口参数：1: 成功，但遇到了复位点
* 出口参数：2: 失败，方向参数错误
* 说    明：电机 200 步 一圈  ，同步带总行程 605个脉冲
* 调用方法：此函数 反向运动，遇到复位点就停止。 正向运动暂时一直运行到指定的脉冲数，所以指定的pluse不要太大，否则会一直运行到底部
****************************************************************************/
uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse)
{
		uint16_t pluse_count = 0;
	  uint32_t speed_count = 0;
	
  	speed_count = speed *100;
 		pluse_count = pluse;
	  GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机
		if(dir)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_1);//方向正向-出仓
		}
		else
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向反向 -进仓
		}
		
		//1.如果是出仓方向，而且已经运行到复位点了，就不在继续转动了
		if((dir==1)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0))  ////读霍尔传感器点 , 更改为Pin_5
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			return 1;//不足pluse_count个脉冲，但遇到了复位点，也返回
		}
		
		//2.开始执行pluse个脉冲
		if(dir==1)//出仓
		{
			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != 0)&&(pluse_count > 0))  //读霍尔传感器点 , 更改为Pin_5
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				pluse_count = pluse_count - 1;
			}			
			
			if(pluse_count==0)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
				return 0;//执行了pluse_count个脉冲
			}
			else
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
				return 1;//不足pluse_count个脉冲，但已经达到复位点
			}
		}
		else if(dir==0)//进仓
		{
// 			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) != 0)&&(pluse_count > 0))
			while((pluse_count > 0))
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				pluse_count = pluse_count - 1;
			}

			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			return 0;//执行了pluse_count个脉冲
			
		}
		else
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			return 2;//方向参数错误
		}
}

/****************************************************************************
* 名    称：uint8_t Plate_Motor_shake(uint8_t dir,uint8_t speed,uint8_t time)
* 功    能：同步带电机运行指定的脉冲数 振动
* 入口参数1：方向 
* 入口参数1：速度    
* 入口参数1：时间
* 出口参数：0:成功
* 说    明：正向：指从复位点到光电池检测的反向
* 调用方法：此函数 反向运动，遇到复位点就停止。 正向运动暂时一直运行到指定的脉冲数
****************************************************************************/
uint8_t Plate_Motor_Shake(uint8_t speed,uint8_t time)
{
		uint16_t pluse_count = 20;
	  uint8_t speed_count =0;
	  uint8_t time_count =0;
	  uint8_t dir = 1;
		u8 ret = 0;

    speed_count = speed;
		time_count  = time;
		
		ret = Plate_Motor_Reset(1);//酶标板先复位
		
	
   	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机
		//酶标板 进仓一段距离后在振板
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
		while((pluse_count > 0))
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			pluse_count = pluse_count - 1;
		}	
		
		pluse_count = 10;
		while(time_count > 0)
		{
			//2.开始执行pluse个脉冲
			if(dir==1)//出仓
			{
				while((pluse_count > 0))
				{
					GPIO_ResetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					GPIO_SetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					pluse_count = pluse_count - 1;
				}	
				dir = 0;				
			}
			else if(dir==0)//进仓
			{
				while(pluse_count > 0)
				{
					GPIO_ResetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					GPIO_SetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					pluse_count = pluse_count - 1;
				}
				dir = 1;				
			}
			time_count = time_count -1;
			pluse_count = 10;
			if(dir)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_1);//方向出仓
			}
			else
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
			}
		}			
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		return ret;
}

/****************************************************************************
* 名    称：uint8_t Plate_Motor_Position(uint8_t speed,uint8_t index)
* 功    能：同步带电机定位
* 入口参数1：定位速度
* 入口参数2：定位索引
* 出口参数：0:定位成功，
*         ：1:定位失败，参数错误
*         ：2:定位失败，复位失败
*         ：3:定位失败，同步带定位光耦坏了
*         ：4:定位失败，硬件故障
* 说    明：酶标板行程 总大约605步
* 调用方法：每次定位都以复位点为参考点
****************************************************************************/
uint8_t Plate_Motor_Position(uint8_t speed,uint8_t index)
{
	uint16_t pluse_Max = 600;
	uint16_t count = 100;
	uint8_t speed_delay = speed;
	uint8_t col = index;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t ret = 0;
	
	if(col <= 0 || col > 12 || speed <=0)
	{
		return 1;//参数不对
	}
	
 	ret = Plate_Motor_Reset(1);
	if(ret == 0)//复位成功
	{
		delay_ms(20);//
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
		
		//让定位片进入光耦
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//光耦挡住是1， 通过为0
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
			delay_ms(speed_delay);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
			delay_ms(speed_delay);
			pluse_Max = pluse_Max - 1;
			count = count - 1;
		}
		
		if(count==0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			G_Error_Type |= 1<<2; //记录异常状态
			return 3;//同步带定位光耦坏了
		}
		
		//正式开始定位
		for(i=0;i<col;i++)
		{
			//这里7个脉冲式为了让挡片移动一点距离，让定位槽偏离光耦，方便检测下一个定位槽
			for(j=0;j<7;j++)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
				delay_ms(speed_delay);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
				delay_ms(speed_delay);
				pluse_Max = pluse_Max - 1;
			}
			
 			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//光耦挡住是1， 通过为0
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
				delay_ms(speed_delay);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 				delay_ms(1);
				delay_ms(speed_delay);
				pluse_Max = pluse_Max - 1;
			}
			
			if(pluse_Max == 0)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
				G_Error_Type |= 1<<4; //记录异常状态
				return 4;//定位失败
			}
		}
	}
	else//复位失败
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		return 2;
	}
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机	
	return 0;//定位成功
}
//----------------------------------------------下面是滤光轮相关函数---------------------------------------------------------------//

/****************************************************************************
* 名    称：void Filter_Motor_Run(uint8_t dir)
* 功    能：滤光轮电机运动
* 入口参数1：方向 ， 1：正向， 0：反向
* 出口参数：无
* 说    明：步进1步 ，400步1圈
* 调用方法：		
****************************************************************************/
void Filter_Motor_Run(uint8_t dir)
{
	if (filter_motor_stop_flag == 1)
	{
		return;
	}
	
	switch(step_index)
	{
		case 0://1000
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 1://1100
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 2://0100
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 3://0110
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 4://0010
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 5://0011
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			break;
		case 6://0001
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			break;
		case 7://1001
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			break;
	}
	
	if(dir)//正向
	{
		step_index = step_index+1;
		if(step_index > 7)
		{
			step_index=0;
		}
	}
	else//反向
	{
		step_index = step_index-1;
		if(step_index < 0)
		{
			step_index=7;
		}
	}
}

/****************************************************************************
* 名    称：uint8_t Filter_Motor_Reset(uint8_t dir,uint8_t speed)
* 功    能：滤光轮电机复位
* 入口参数：
* 出口参数: 0：复位成功， 1：复位失败 
* 说    明：
* 调用方法：400 个脉冲一圈 
****************************************************************************/
uint8_t Filter_Motor_Reset(uint8_t dir,uint8_t speed)
{
	uint16_t step_count = 500;//最大的测试步进数
	uint8_t speed_delay = speed;
	
	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) != 0)&&(step_count > 0))
	{
		Filter_Motor_Run(dir);//滤光轮电机步进一步
// 		delay_ms(2);//延时2ms
		delay_ms(speed_delay);
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		step_count = step_count -1;
	}
	
	if(step_count==0)
	{
		G_Error_Type |= 1<<5;
		return 1;//复位失败
	}
	else
	{
		G_Error_Type &= ~(1<<5);
		return 0;//复位成功
	}
}

/****************************************************************************
* 名    称：uint8_t Filter_Motor_Location(uint8_t dir,uint8_t speed,uint8_t position)
* 功    能：滤光轮电机定位
* 入口参数1：dir方向， 1：正向 ，0:反向
* 入口参数2：速度
* 入口参数3：position位置，滤光片位置，0：复位，1：405nm, 2:450nm, 3:492nm, 4:630nm
* 出口参数: 0：复位成功， 1：复位失败 ，2：position参数错误
* 说    明：
* 调用方法：400 个脉冲一圈 
****************************************************************************/
uint8_t Filter_Motor_Location(uint8_t dir,uint8_t speed,uint8_t position)
{
	uint16_t step_count = 0;//
	uint16_t speed_delay = speed;//
	uint8_t error =0;
	//1.滤光轮先复位
	error = Filter_Motor_Reset(dir,speed_delay);
	if(error)
	{
		return 1;//滤光轮复位失败
	}
	//2.确定定位脉冲数
	switch(position)
	{
		case 0://复位点
			step_count = 0;
			break;
		case 1://405nm
			step_count = 50;
			break;
		case 2://450nm
			step_count = 100;
			break;
		case 3://492nm
			step_count = 150;
			break;
		case 4://630nm
			step_count = 200;
			break;
		case 5://未使用
			step_count = 250;
			break;
		case 6://未使用
			step_count = 300;
			break;
		case 7://未使用
			step_count = 350;
			break;
		case 8://未使用
			step_count = 400;
			break;
		default:
			break;
	}
	//3.定位
	while(step_count > 0)
	{
		Filter_Motor_Run(dir);//滤光轮电机步进一步
// 		delay_ms(2);//延时2ms
		delay_ms(speed_delay);//
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);

		step_count = step_count -1;
	}
	return 0;
}

//----------------------------------------------下面是同步带电机和滤光轮电机 组合功能---------------------------------------------------------------//
/****************************************************************************
* 名    称：uint8_t Filter_Plate_Motor_Loop(u8 mode,u8 speed)
* 功    能：轮询定位
* 入口参数1：dir方向， 1：正向 ，0:反向
* 入口参数2：position位置，滤光片位置，0：复位，1：405nm, 2:450nm, 3:492nm, 4:630nm
* 出口参数: 0：复位成功， 1：复位失败 ，2：position参数错误
* 说    明：
* 调用方法：
****************************************************************************/
 uint8_t Filter_Plate_Motor_Loop(u8 mode,u8 speed)
{
	uint8_t ret = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t wave_length = 1;
	uint16_t pluse_Max = 600;
	uint8_t count = 100;
// 	u8 Response_CMD[2]={0x28,0xff};
	
	ret = Filter_Motor_Location(1,5,wave_length);
	if(ret)//复位失败
	{
		return 1;
	}
	
	//酶标板复位
	ret = Plate_Motor_Reset(1);
	if(ret)//复位失败
	{
		return 1;
	}
		// 		//滤光轮定位到 1号波长
	// 		wave_length = i%8;
	// 		Filter_Motor_Location(1,3,wave_length);

	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
	
	//让定位片进入光耦
	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//光耦挡住是1， 通过为0
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_2);
		delay_ms(1);
		GPIO_SetBits(GPIOB,GPIO_Pin_2);
		delay_ms(1);
		pluse_Max = pluse_Max - 1;
		count = count - 1;
	}
	
	if(count==0)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		return 2;//同步带定位光耦有问题
	}	
		
	if(mode==0)//步进
	{
		//正式开始定位12列
		for(i=0;i<12;i++)
		{
			//这里7个脉冲式为了让挡片移动一点距离，让定位槽偏离光耦，方便检测下一个定位槽
			for(j=0;j<7;j++)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				pluse_Max = pluse_Max - 1;
			}
			
			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//光耦挡住是1， 通过为0
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				pluse_Max = pluse_Max - 1;
			}
			
			if(pluse_Max == 0)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
				return 3;//定位失败
			}

			//定位完1列
			AD_Sample_Mode1(i);
	// 			Get_Adc_Mode0(i);
		
		}
	}
	else//连续
	{
		//正式开始定位12列
		for(i=0;i<12;i++)
		{
			//这里7个脉冲式为了让挡片移动一点距离，让定位槽偏离光耦，方便检测下一个定位槽
			for(j=0;j<7;j++)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				pluse_Max = pluse_Max - 1;
			}
			
			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//光耦挡住是1， 通过为0
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				pluse_Max = pluse_Max - 1;
			}
			
			if(pluse_Max == 0)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
				return 3;//定位失败
			}

			//定位完1列
// 			AD_Sample_Mode1(i);
	 			Get_Adc_Mode0(i,speed);
		
		}
	}
	
	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
	delay_ms(2);

	//复位
	ret = Plate_Motor_Reset(1);
	if(ret)//复位失败
	{
		return 1;
	}
	
	ret = Filter_Motor_Reset(1,5);
	if(ret)//复位失败
	{
		return 4;
	}
	
	return 0;
}

