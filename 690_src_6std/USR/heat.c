#include "heat.h"
#include "ds18b20.h"


struct PID {
        float SetPoint; // 设定目标 Desired Value
        float Proportion; // 比例常数 Proportional Const
        float Integral; // 积分常数 Integral Const
        float Derivative; // 微分常数 Derivative Const
        float LastError; // Error[-1]
        float PrevError; // Error[-2]
        float SumError; // Sums of Errors
        };




struct PID spid,rpid;    // PID Control Structure
uint16_t rout;  // PID Response (Output)
uint16_t rin;   // PID Feedback (Input)


uint16_t pre_cold_temp = 0xffff;//记录上次一的温度
uint16_t pre_temp = 0x0; 
u8 flag = 0;
/****************************************************************************
* 名    称：void PIDInit (struct PID *pp)
* 功    能：初始化数据结构
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void PIDInit (struct PID *pp,u8 flag)
{
  memset ( pp,0,sizeof(struct PID));           //全部初始化为0
	if(flag==HEAT3)
	{
	      spid.Proportion = 15.0; //设置PID 比例值
        spid.Integral = 0.25; //设置PID 积分值
        spid.Derivative = 7.0; //设置PID 微分值
	}
	else if(flag==HEAT1)                                  //需要另外一组参数
	{      
		    rpid.Proportion = 0.9; //设置PID 比例值
        rpid.Integral = 0.15; //设置PID 积分值
        rpid.Derivative = 2.0; //设置PID 微分值
	}
	      
}



/****************************************************************************
* 名    称：uint16_t PIDCalc( struct PID *pp, uint16_t NextPoint )
* 功    能：初始化数据结构
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
float PIDCalc(struct PID *pp,uint16_t NextPoint)
{
        float dError,Error;
	
        Error = pp->SetPoint - NextPoint;          // 偏差           
        pp->SumError += Error;                     // 积分                                   
        dError = pp->LastError - pp->PrevError;    // 当前微分  
        pp->PrevError = pp->LastError;                           
        pp->LastError = Error;                                        
        return (pp->Proportion * Error             // 比例项           
        + pp->Integral * pp->SumError              // 积分项
        + pp->Derivative * dError);                // 微分项
}


/****************************************************************************
* 名    称：void Heat3_Init(void)
* 功    能：TIMER1的PWM初始化
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/

void Heat3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;




  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;      //7-->heat3     6-->heat2  8-->heat1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	

	

	
	TIM_TimeBaseStructure.TIM_Period = 199;     //定时周期1000  ，  20KHZ 周期为 50us, 200个计数后溢出， 即 定时器周期 为 10000US =10MS
  TIM_TimeBaseStructure.TIM_Prescaler = 2399; // 定时器时钟为 48MHZ , 分频2400后得到 20KHZ， 
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
  TIM_OCInitStructure.TIM_Pulse = 200;                           //占空比 0-->100%  200--->0%  
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);   
	
	                              

  TIM_Cmd(TIM4, ENABLE); 
              
	PIDInit(&spid,HEAT3);
}



/****************************************************************************
* 名    称：void Peltier_Init(u8 state)
* 功    能：帕尔贴驱动初始化
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void Peltier_Init(u8 state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	if(state==HOTSTATE)      //发热
	{
		
		//发热时，heat2 恒为低电平
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);               //低电平
		
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		TIM_TimeBaseStructure.TIM_Period = 199;     //定时周期1000
    TIM_TimeBaseStructure.TIM_Prescaler = 2399;           
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		
		
		//heat1-->pin8-->channel3
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = 200;                           //占空比 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
		
		TIM_Cmd(TIM4, ENABLE); 

		PIDInit(&rpid,HEAT1);
	}
	else if(state==COLDSTATE)        //制冷
	{	
				//制冷时，heat1 恒为低电平
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);               //低电平
		
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		TIM_TimeBaseStructure.TIM_Period = 199;     //定时周期1000
    TIM_TimeBaseStructure.TIM_Prescaler = 2399;           
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		
		
		//heat2-->pin6-->channel1
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = 200;                           //占空比 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC1Init(TIM4, &TIM_OCInitStructure);   
		
		TIM_Cmd(TIM4, ENABLE); 

		PIDInit(&rpid,HEAT1);
		}
	else
	{
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_8;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);               //低电平
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);               //低电平
	}
}




/****************************************************************************
* 名    称：uint8_t HEAT3_Hot(uint8_t time,uint8_t delay)
* 功    能：TIMER1的PWM初始化
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void Pwm_Driver(uint8_t heater,float pid_out)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	 
	uint8_t Plus;

	
	if(heater==HEAT3)  //加热片
	{
		
    //加热片是增量PID
	  //pid_out值越大，说明温差大，占空比要变大，设置值要变小
	  //pid_out值越小，说明温差小，占空比要变小，设置值要变大
		if(pid_out>=200)   //温度亏欠太多了，需大补
			Plus = 0;
		else if(pid_out>0 && pid_out<200)   //中部
		   Plus = 200 - pid_out;
    else		
			 Plus = 200;		
	
		 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = Plus;                           //占空比 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC2Init(TIM4, &TIM_OCInitStructure);   
	}
	else if(heater==HEAT1)          //帕尔贴加热
	{
		//加热片是增量PID
	  //pid_out值越大，说明温差大，占空比要变大，设置值要变小
	  //pid_out值越小，说明温差小，占空比要变小，设置值要变大
		if(pid_out>=200)   //温度亏欠太多了，需大补,为了保护帕尔贴不损坏，最大的占空比为50%
			Plus = 100;
		else if(pid_out>0 && pid_out<200)   //中部
		   Plus = 200 - pid_out/2;
    else		
			 Plus = 200;		
	
		 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = 200-Plus;                           //占空比 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
	}
	else ////帕尔贴制冷
	{
		if(pid_out>=200)   //温度亏欠太多了，需大补
			Plus = 100;
		else if(pid_out>0 && pid_out<200)   //中部
		   Plus = 200 - pid_out/2;
    else		
			 Plus = 200;		
	
		 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
//     TIM_OCInitStructure.TIM_Pulse = Plus;                           //占空比 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_Pulse = 200-Plus;                           //占空比 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC1Init(TIM4, &TIM_OCInitStructure);   
	}
	
}




/****************************************************************************
* 名    称：void Pwm_Driver_All(uint8_t heater,uint8_t flag)
* 功    能：TIMER1的PWM初始化
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void Pwm_Driver_All(uint8_t heater,uint8_t state)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	if(heater==HEAT3)
	{
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
	  if(state==HOTSTATE)                                                      //全速加热
		  TIM_OCInitStructure.TIM_Pulse = 0;                             //占空比 0-->100%  200--->0%  
	  else if(state==IDLESTATE)
      TIM_OCInitStructure.TIM_Pulse = 200;                           //占空比 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC2Init(TIM4, &TIM_OCInitStructure);   
  }
	else if(heater==HEAT1)
	{
		    
	  if(state==HOTSTATE)                                       //全速加热
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);                      //低电平
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
		  TIM_OCInitStructure.TIM_Pulse = 100;                             //占空比 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
		}
   else if(state==IDLESTATE)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);                      //低电平
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
// 		  TIM_OCInitStructure.TIM_Pulse = 200;                             //占空比 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_Pulse = 0;                        
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
		}
		
	}
	else                                                               //制冷
	{
		if(state==COLDSTATE)                                       //全速制冷
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);                      //低电平
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
		  TIM_OCInitStructure.TIM_Pulse = 100;                             //占空比 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC1Init(TIM4, &TIM_OCInitStructure);   
		}
   else if(state==IDLESTATE)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);                      //低电平
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
		  TIM_OCInitStructure.TIM_Pulse = 0;                             //占空比 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC1Init(TIM4, &TIM_OCInitStructure);   
		}
	}

}



/****************************************************************************
* 名    称：void Heat3_Control(uint16_t Target_Tem)
* 功    能：加热片加热控制
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void Heat3_Control(uint16_t Target_Tem,u8 State)
{
	  u16 temp=0;
	
	  if(State==HOTSTATE)
		{
			temp=DS18B20_Get_wd(1);
			temp = Temperature_convert(temp);
			if(G_Temperature_Disp == 2)
			{
				if (temp != 850)
				{
					printf(":%.1f:2:",temp/10.0);
				}
			}
// 			printf("1 %d - %d\r\n",Target_Tem,temp);
			if(Target_Tem-temp>10)                   //温差是否是大于1度
				Pwm_Driver_All(HEAT3,HOTSTATE);                  //如果是，则全速加热
			else if(Target_Tem-temp>5)      
			{
					spid.SetPoint = Target_Tem;
					rin = temp;                                  // Read Input
					rout = PIDCalc (&spid,rin );                 // Perform PID Interation
// 					printf("1 %d - %d - %d\r\n",Target_Tem,temp,rout);
					Pwm_Driver(HEAT3,rout);                          //根据rout值调节占空比
			}
			else
			{
					Pwm_Driver(HEAT3,0);                          //根据rout值调节占空比		
			}
		}
		else if(State==IDLESTATE)
			Pwm_Driver_All(HEAT3,IDLESTATE);                  //如果是，停止加热
}

/****************************************************************************
* 名    称：void Heat1_Control(uint16_t Target_Tem)
* 功    能：帕尔贴加热控制
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void Heat1_Control(uint16_t Target_Tem,u8 State)
{
	u16 temp=0;
	
	if(State==HOTSTATE)
	{
	  temp=DS18B20_Get_wd(2);
	  temp = Temperature_convert(temp);
		if(G_Temperature_Disp == 3)
		{
			if (temp != 850)
			{
				printf(":%.1f:3:",temp/10.0);
			}
		}
		
		if(Target_Tem-temp>20)                              //温差是否是大于2度
		{
// 				printf("2 %d -%d\r\n",Target_Tem,temp);
				Pwm_Driver_All(HEAT1,HOTSTATE);                  //如果是，则全速加热
		}
		else if(Target_Tem-temp>10)
		{
				rpid.SetPoint = Target_Tem;
				rin = temp;                                        // Read Input
				rout = PIDCalc (&rpid,rin );                        // Perform PID Interation
// 				printf("2 %d -%d - %d\r\n",Target_Tem,temp,rout);
				Pwm_Driver(HEAT1,rout);                          //根据rout值调节占空比
		}
		else		
    {
				rpid.SetPoint = Target_Tem;
				rin = temp;                                        // Read Input
				rout = PIDCalc (&rpid,rin );                        // Perform PID Interation
				rout = 0;
// 				printf("2 %d -%d - %d\r\n",Target_Tem,temp,rout);
				Pwm_Driver(HEAT1,rout);                          //根据rout值调节占空比
				
				
		}
	 }
	 else if(State==IDLESTATE)
	   Pwm_Driver_All(HEAT1,IDLESTATE);                  //如果是，则全速加热
	 
}

/****************************************************************************
* 名    称：void Heat1_Control(uint16_t Target_Tem)
* 功    能：帕尔贴制冷控制
* 入口参数1：无
* 出口参数：
* 说    明：制冷时，一面制冷，另一面制热，如果另一面制热的温度不能及时排除，会把热量传递到制冷面，这导致最后温度反而会上升。
* 调用方法：
****************************************************************************/
void Heat2_Control(uint16_t Target_Tem,u8 State)
{
	u16 temp=0;
	
  if(State==COLDSTATE)
	{
		temp=DS18B20_Get_wd(2);
	  temp = Temperature_convert(temp);
		if(G_Temperature_Disp == 3)
		{
			if (temp != 850)
			{
				printf(":%.1f:3:",temp/10.0);
			}
		}
	  if(Target_Tem<temp)                              //温差是否是大于1度
		{
			if(temp <= pre_cold_temp)
			{
				flag = 0;
				pre_temp = 0;
				//如果当前检测的温度比之前的要低，说明制冷有效果，更新之前的温度
				pre_cold_temp = temp;
				Pwm_Driver_All(HEAT2,COLDSTATE);                  
// 				printf("3-1 %d %d %d\r\n",Target_Tem,temp,pre_cold_temp);
			}
			else
			{
				if(temp >= pre_temp)
				{
					pre_temp = temp;
				}
				else
				{
					//停止制冷后，对面的高温会立即传导过来，导致温度会很快升高，此时现象是，制冷面温度上升，上升到一定高度后，慢慢开始下降。制热面慢慢下降，
					//因此停止制冷后，制冷面的温度会升高好几度，观察能达到6度，然后再下降。此时要调整之前的温度值，下次制冷的参考温度提高2度
					//整个制冷过程如下：比如 从 48 制冷到 42，下降6度后，关闭制冷，温度上升下降然后从44度开始制冷
					if((pre_temp - pre_cold_temp > 30)&&(flag==0))
					{
						flag = 1;
						pre_cold_temp += 20;
					}
				}
				
				//此时说明温度没有下降，说明制冷效果不好，可能是另一面的热量影响到了这一面，此时应该停止制冷动作，否则温度会越来越高
        Pwm_Driver_All(HEAT2,IDLESTATE);                   
// 				printf("3-2 %d %d %d\r\n",Target_Tem,temp,pre_cold_temp);
			}
		}
    else   
		{
      Pwm_Driver_All(HEAT2,IDLESTATE);                  //如果是，则全速加热                         //根据rout值调节占空比
// 			printf("3-3 %d %d %d\r\n",Target_Tem,temp,pre_cold_temp);
		}			
	 }
	 else if(State==IDLESTATE)
	   Pwm_Driver_All(HEAT2,IDLESTATE);                  //如果是，则全速加热

}




/****************************************************************************
* 名    称：void compare_temper(uint16_t Target_Tem)
* 功    能：初始化数据结构
* 入口参数1：无
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
// void compare_temper(uint16_t Target_Tem,u8 Heater)
// {
//        
// 	     u16 temp=0;
// 	     uint8_t k =0;

// 	
// 	    if(Heater==HEAT3)
// 			{
// 	       
// 				 temp=DS18B20_Get_wd(1);
//          temp = Temperature_convert(temp);
//  	       if(Target_Tem-temp>10)                   //温差是否是大于1度
//             Pwm_Driver_All(HEAT3,HOTSTATE);                  //如果是，则全速加热
//          else       
//           {
//             spid.SetPoint = Target_Tem;
//             rin = temp;                                  // Read Input
//             rout = PIDCalc (&spid,rin );                 // Perform PID Interation
// 						printf("%d %d\r\n",temp,rout);
// 						Pwm_Driver(HEAT3,rout);                          //根据rout值调节占空比
//           }
// 			 }
// 			 else if(Heater==HEAT1)
// 			 {
// 				 temp=DS18B20_Get_wd(2);
// 				 temp = Temperature_convert(temp);
// 				 if(Target_Tem-temp>30)                              //温差是否是大于1度
//             Pwm_Driver_All(HEAT1,HOTSTATE);                  //如果是，则全速加热
//          else       
//           {
//             spid.SetPoint = Target_Tem;
//             rin = temp;                                        // Read Input
//             rout = PIDCalc (&rpid,rin );                        // Perform PID Interation
// 						printf("%d %d\r\n",temp,rout);
// 						Pwm_Driver(HEAT1,rout);                          //根据rout值调节占空比
//           }
// 			 }
// 			 else                                                   //制冷
// 			 {
// 				  temp=DS18B20_Get_wd(2);
// 				  temp = Temperature_convert(temp);
// 				  if(Target_Tem<temp)                               //目标温度低于实际温度
// 					{
// 						Pwm_Driver_All(HEAT2,COLDSTATE);                  //如果是，则全速制冷
// 						Pwm_Driver_All(HEAT3,IDLESTATE);                  //如果是，不加热
// 					}
// 					else
// 					{
// 						Pwm_Driver_All(HEAT2,IDLESTATE);                  //如果是，停止制冷
// 					  Pwm_Driver_All(HEAT3,IDLESTATE);                  //如果是，不加热
// 					}
// 			 }
// 	

// 				
// 			
// }




