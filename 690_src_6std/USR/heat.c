#include "heat.h"
#include "ds18b20.h"


struct PID {
        float SetPoint; // �趨Ŀ�� Desired Value
        float Proportion; // �������� Proportional Const
        float Integral; // ���ֳ��� Integral Const
        float Derivative; // ΢�ֳ��� Derivative Const
        float LastError; // Error[-1]
        float PrevError; // Error[-2]
        float SumError; // Sums of Errors
        };




struct PID spid,rpid;    // PID Control Structure
uint16_t rout;  // PID Response (Output)
uint16_t rin;   // PID Feedback (Input)


uint16_t pre_cold_temp = 0xffff;//��¼�ϴ�һ���¶�
uint16_t pre_temp = 0x0; 
u8 flag = 0;
/****************************************************************************
* ��    �ƣ�void PIDInit (struct PID *pp)
* ��    �ܣ���ʼ�����ݽṹ
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void PIDInit (struct PID *pp,u8 flag)
{
  memset ( pp,0,sizeof(struct PID));           //ȫ����ʼ��Ϊ0
	if(flag==HEAT3)
	{
	      spid.Proportion = 15.0; //����PID ����ֵ
        spid.Integral = 0.25; //����PID ����ֵ
        spid.Derivative = 7.0; //����PID ΢��ֵ
	}
	else if(flag==HEAT1)                                  //��Ҫ����һ�����
	{      
		    rpid.Proportion = 0.9; //����PID ����ֵ
        rpid.Integral = 0.15; //����PID ����ֵ
        rpid.Derivative = 2.0; //����PID ΢��ֵ
	}
	      
}



/****************************************************************************
* ��    �ƣ�uint16_t PIDCalc( struct PID *pp, uint16_t NextPoint )
* ��    �ܣ���ʼ�����ݽṹ
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
float PIDCalc(struct PID *pp,uint16_t NextPoint)
{
        float dError,Error;
	
        Error = pp->SetPoint - NextPoint;          // ƫ��           
        pp->SumError += Error;                     // ����                                   
        dError = pp->LastError - pp->PrevError;    // ��ǰ΢��  
        pp->PrevError = pp->LastError;                           
        pp->LastError = Error;                                        
        return (pp->Proportion * Error             // ������           
        + pp->Integral * pp->SumError              // ������
        + pp->Derivative * dError);                // ΢����
}


/****************************************************************************
* ��    �ƣ�void Heat3_Init(void)
* ��    �ܣ�TIMER1��PWM��ʼ��
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
	

	

	
	TIM_TimeBaseStructure.TIM_Period = 199;     //��ʱ����1000  ��  20KHZ ����Ϊ 50us, 200������������� �� ��ʱ������ Ϊ 10000US =10MS
  TIM_TimeBaseStructure.TIM_Prescaler = 2399; // ��ʱ��ʱ��Ϊ 48MHZ , ��Ƶ2400��õ� 20KHZ�� 
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
  TIM_OCInitStructure.TIM_Pulse = 200;                           //ռ�ձ� 0-->100%  200--->0%  
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);   
	
	                              

  TIM_Cmd(TIM4, ENABLE); 
              
	PIDInit(&spid,HEAT3);
}



/****************************************************************************
* ��    �ƣ�void Peltier_Init(u8 state)
* ��    �ܣ�������������ʼ��
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void Peltier_Init(u8 state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	if(state==HOTSTATE)      //����
	{
		
		//����ʱ��heat2 ��Ϊ�͵�ƽ
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);               //�͵�ƽ
		
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		TIM_TimeBaseStructure.TIM_Period = 199;     //��ʱ����1000
    TIM_TimeBaseStructure.TIM_Prescaler = 2399;           
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		
		
		//heat1-->pin8-->channel3
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = 200;                           //ռ�ձ� 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
		
		TIM_Cmd(TIM4, ENABLE); 

		PIDInit(&rpid,HEAT1);
	}
	else if(state==COLDSTATE)        //����
	{	
				//����ʱ��heat1 ��Ϊ�͵�ƽ
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);               //�͵�ƽ
		
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;      //6-->heat2  8-->heat1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		TIM_TimeBaseStructure.TIM_Period = 199;     //��ʱ����1000
    TIM_TimeBaseStructure.TIM_Prescaler = 2399;           
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		
		
		//heat2-->pin6-->channel1
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = 200;                           //ռ�ձ� 0-->100%  200--->0%  
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
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);               //�͵�ƽ
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);               //�͵�ƽ
	}
}




/****************************************************************************
* ��    �ƣ�uint8_t HEAT3_Hot(uint8_t time,uint8_t delay)
* ��    �ܣ�TIMER1��PWM��ʼ��
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void Pwm_Driver(uint8_t heater,float pid_out)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	 
	uint8_t Plus;

	
	if(heater==HEAT3)  //����Ƭ
	{
		
    //����Ƭ������PID
	  //pid_outֵԽ��˵���²��ռ�ձ�Ҫ�������ֵҪ��С
	  //pid_outֵԽС��˵���²�С��ռ�ձ�Ҫ��С������ֵҪ���
		if(pid_out>=200)   //�¶ȿ�Ƿ̫���ˣ����
			Plus = 0;
		else if(pid_out>0 && pid_out<200)   //�в�
		   Plus = 200 - pid_out;
    else		
			 Plus = 200;		
	
		 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = Plus;                           //ռ�ձ� 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC2Init(TIM4, &TIM_OCInitStructure);   
	}
	else if(heater==HEAT1)          //����������
	{
		//����Ƭ������PID
	  //pid_outֵԽ��˵���²��ռ�ձ�Ҫ�������ֵҪ��С
	  //pid_outֵԽС��˵���²�С��ռ�ձ�Ҫ��С������ֵҪ���
		if(pid_out>=200)   //�¶ȿ�Ƿ̫���ˣ����,Ϊ�˱������������𻵣�����ռ�ձ�Ϊ50%
			Plus = 100;
		else if(pid_out>0 && pid_out<200)   //�в�
		   Plus = 200 - pid_out/2;
    else		
			 Plus = 200;		
	
		 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
    TIM_OCInitStructure.TIM_Pulse = 200-Plus;                           //ռ�ձ� 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
	}
	else ////����������
	{
		if(pid_out>=200)   //�¶ȿ�Ƿ̫���ˣ����
			Plus = 100;
		else if(pid_out>0 && pid_out<200)   //�в�
		   Plus = 200 - pid_out/2;
    else		
			 Plus = 200;		
	
		 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
//     TIM_OCInitStructure.TIM_Pulse = Plus;                           //ռ�ձ� 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_Pulse = 200-Plus;                           //ռ�ձ� 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC1Init(TIM4, &TIM_OCInitStructure);   
	}
	
}




/****************************************************************************
* ��    �ƣ�void Pwm_Driver_All(uint8_t heater,uint8_t flag)
* ��    �ܣ�TIMER1��PWM��ʼ��
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void Pwm_Driver_All(uint8_t heater,uint8_t state)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	if(heater==HEAT3)
	{
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        
	  if(state==HOTSTATE)                                                      //ȫ�ټ���
		  TIM_OCInitStructure.TIM_Pulse = 0;                             //ռ�ձ� 0-->100%  200--->0%  
	  else if(state==IDLESTATE)
      TIM_OCInitStructure.TIM_Pulse = 200;                           //ռ�ձ� 0-->100%  200--->0%  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	  TIM_OC2Init(TIM4, &TIM_OCInitStructure);   
  }
	else if(heater==HEAT1)
	{
		    
	  if(state==HOTSTATE)                                       //ȫ�ټ���
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);                      //�͵�ƽ
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
		  TIM_OCInitStructure.TIM_Pulse = 100;                             //ռ�ձ� 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
		}
   else if(state==IDLESTATE)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_6);                      //�͵�ƽ
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
// 		  TIM_OCInitStructure.TIM_Pulse = 200;                             //ռ�ձ� 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_Pulse = 0;                        
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC3Init(TIM4, &TIM_OCInitStructure);   
		}
		
	}
	else                                                               //����
	{
		if(state==COLDSTATE)                                       //ȫ������
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);                      //�͵�ƽ
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
		  TIM_OCInitStructure.TIM_Pulse = 100;                             //ռ�ձ� 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC1Init(TIM4, &TIM_OCInitStructure);   
		}
   else if(state==IDLESTATE)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);                      //�͵�ƽ
			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    
		  TIM_OCInitStructure.TIM_Pulse = 0;                             //ռ�ձ� 0-->100%  200--->0%  
		  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         
	    TIM_OC1Init(TIM4, &TIM_OCInitStructure);   
		}
	}

}



/****************************************************************************
* ��    �ƣ�void Heat3_Control(uint16_t Target_Tem)
* ��    �ܣ�����Ƭ���ȿ���
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
			if(Target_Tem-temp>10)                   //�²��Ƿ��Ǵ���1��
				Pwm_Driver_All(HEAT3,HOTSTATE);                  //����ǣ���ȫ�ټ���
			else if(Target_Tem-temp>5)      
			{
					spid.SetPoint = Target_Tem;
					rin = temp;                                  // Read Input
					rout = PIDCalc (&spid,rin );                 // Perform PID Interation
// 					printf("1 %d - %d - %d\r\n",Target_Tem,temp,rout);
					Pwm_Driver(HEAT3,rout);                          //����routֵ����ռ�ձ�
			}
			else
			{
					Pwm_Driver(HEAT3,0);                          //����routֵ����ռ�ձ�		
			}
		}
		else if(State==IDLESTATE)
			Pwm_Driver_All(HEAT3,IDLESTATE);                  //����ǣ�ֹͣ����
}

/****************************************************************************
* ��    �ƣ�void Heat1_Control(uint16_t Target_Tem)
* ��    �ܣ����������ȿ���
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
		
		if(Target_Tem-temp>20)                              //�²��Ƿ��Ǵ���2��
		{
// 				printf("2 %d -%d\r\n",Target_Tem,temp);
				Pwm_Driver_All(HEAT1,HOTSTATE);                  //����ǣ���ȫ�ټ���
		}
		else if(Target_Tem-temp>10)
		{
				rpid.SetPoint = Target_Tem;
				rin = temp;                                        // Read Input
				rout = PIDCalc (&rpid,rin );                        // Perform PID Interation
// 				printf("2 %d -%d - %d\r\n",Target_Tem,temp,rout);
				Pwm_Driver(HEAT1,rout);                          //����routֵ����ռ�ձ�
		}
		else		
    {
				rpid.SetPoint = Target_Tem;
				rin = temp;                                        // Read Input
				rout = PIDCalc (&rpid,rin );                        // Perform PID Interation
				rout = 0;
// 				printf("2 %d -%d - %d\r\n",Target_Tem,temp,rout);
				Pwm_Driver(HEAT1,rout);                          //����routֵ����ռ�ձ�
				
				
		}
	 }
	 else if(State==IDLESTATE)
	   Pwm_Driver_All(HEAT1,IDLESTATE);                  //����ǣ���ȫ�ټ���
	 
}

/****************************************************************************
* ��    �ƣ�void Heat1_Control(uint16_t Target_Tem)
* ��    �ܣ��������������
* ��ڲ���1����
* ���ڲ�����
* ˵    ��������ʱ��һ�����䣬��һ�����ȣ������һ�����ȵ��¶Ȳ��ܼ�ʱ�ų�������������ݵ������棬�⵼������¶ȷ�����������
* ���÷�����
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
	  if(Target_Tem<temp)                              //�²��Ƿ��Ǵ���1��
		{
			if(temp <= pre_cold_temp)
			{
				flag = 0;
				pre_temp = 0;
				//�����ǰ�����¶ȱ�֮ǰ��Ҫ�ͣ�˵��������Ч��������֮ǰ���¶�
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
					//ֹͣ����󣬶���ĸ��»��������������������¶Ȼ�ܿ����ߣ���ʱ�����ǣ��������¶�������������һ���߶Ⱥ�������ʼ�½��������������½���
					//���ֹͣ�������������¶Ȼ����ߺü��ȣ��۲��ܴﵽ6�ȣ�Ȼ�����½�����ʱҪ����֮ǰ���¶�ֵ���´�����Ĳο��¶����2��
					//��������������£����� �� 48 ���䵽 42���½�6�Ⱥ󣬹ر����䣬�¶������½�Ȼ���44�ȿ�ʼ����
					if((pre_temp - pre_cold_temp > 30)&&(flag==0))
					{
						flag = 1;
						pre_cold_temp += 20;
					}
				}
				
				//��ʱ˵���¶�û���½���˵������Ч�����ã���������һ�������Ӱ�쵽����һ�棬��ʱӦ��ֹͣ���䶯���������¶Ȼ�Խ��Խ��
        Pwm_Driver_All(HEAT2,IDLESTATE);                   
// 				printf("3-2 %d %d %d\r\n",Target_Tem,temp,pre_cold_temp);
			}
		}
    else   
		{
      Pwm_Driver_All(HEAT2,IDLESTATE);                  //����ǣ���ȫ�ټ���                         //����routֵ����ռ�ձ�
// 			printf("3-3 %d %d %d\r\n",Target_Tem,temp,pre_cold_temp);
		}			
	 }
	 else if(State==IDLESTATE)
	   Pwm_Driver_All(HEAT2,IDLESTATE);                  //����ǣ���ȫ�ټ���

}




/****************************************************************************
* ��    �ƣ�void compare_temper(uint16_t Target_Tem)
* ��    �ܣ���ʼ�����ݽṹ
* ��ڲ���1����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
//  	       if(Target_Tem-temp>10)                   //�²��Ƿ��Ǵ���1��
//             Pwm_Driver_All(HEAT3,HOTSTATE);                  //����ǣ���ȫ�ټ���
//          else       
//           {
//             spid.SetPoint = Target_Tem;
//             rin = temp;                                  // Read Input
//             rout = PIDCalc (&spid,rin );                 // Perform PID Interation
// 						printf("%d %d\r\n",temp,rout);
// 						Pwm_Driver(HEAT3,rout);                          //����routֵ����ռ�ձ�
//           }
// 			 }
// 			 else if(Heater==HEAT1)
// 			 {
// 				 temp=DS18B20_Get_wd(2);
// 				 temp = Temperature_convert(temp);
// 				 if(Target_Tem-temp>30)                              //�²��Ƿ��Ǵ���1��
//             Pwm_Driver_All(HEAT1,HOTSTATE);                  //����ǣ���ȫ�ټ���
//          else       
//           {
//             spid.SetPoint = Target_Tem;
//             rin = temp;                                        // Read Input
//             rout = PIDCalc (&rpid,rin );                        // Perform PID Interation
// 						printf("%d %d\r\n",temp,rout);
// 						Pwm_Driver(HEAT1,rout);                          //����routֵ����ռ�ձ�
//           }
// 			 }
// 			 else                                                   //����
// 			 {
// 				  temp=DS18B20_Get_wd(2);
// 				  temp = Temperature_convert(temp);
// 				  if(Target_Tem<temp)                               //Ŀ���¶ȵ���ʵ���¶�
// 					{
// 						Pwm_Driver_All(HEAT2,COLDSTATE);                  //����ǣ���ȫ������
// 						Pwm_Driver_All(HEAT3,IDLESTATE);                  //����ǣ�������
// 					}
// 					else
// 					{
// 						Pwm_Driver_All(HEAT2,IDLESTATE);                  //����ǣ�ֹͣ����
// 					  Pwm_Driver_All(HEAT3,IDLESTATE);                  //����ǣ�������
// 					}
// 			 }
// 	

// 				
// 			
// }




