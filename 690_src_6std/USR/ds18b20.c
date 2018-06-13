#include "pbdata.h"
#include "ds18b20.h"







/****************************************************************************
* 名    称：u8 DS18B20_Init(void)
* 功    能：温度传感器初始化
* 入口参数：
* 出口参数：返回1:不存在 , 返回0:存在    	
* 说    明：初始化DS18B20的IO口 DQ 同时检测DS的存在
* 调用方法：
****************************************************************************/
u8 DS18B20_Init(u8 channel)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	if(channel==1)
	{
 	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PORTG口时钟 
	  GPIO_InitStructure.GPIO_Pin = IO_DS18B20_1;				    
 	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	  GPIO_Init(GPIO_DS18B20_1, &GPIO_InitStructure);
    GPIO_SetBits(GPIO_DS18B20_1,IO_DS18B20_1);    //输出1
    DS18B20_Reset(1);
    return DS18B20_Check(1);
	}else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能端口D的时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);       //映射PD01的使能
		
	  GPIO_InitStructure.GPIO_Pin   = IO_DS18B20_2;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	  GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStructure);
	  //GPIO_ResetBits(GPIO_DS18B20_2,IO_DS18B20_2);    //输出1
		//delay_ms(1000);
		GPIO_SetBits(GPIO_DS18B20_2,IO_DS18B20_2);    //输出1
		DS18B20_Reset(2);
		return DS18B20_Check(2);
	}
}  



/****************************************************************************
* 名    称：DS18B20_IO_IN
* 功    能：输入
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void DS18B20_IO_IN(u8 channel)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if(channel==1)
	{
	  GPIO_InitStructure.GPIO_Pin=IO_DS18B20_1;
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	  GPIO_Init(GPIO_DS18B20_1,&GPIO_InitStructure);
	}else
	{	  
		GPIO_InitStructure.GPIO_Pin=IO_DS18B20_2;
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	  GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStructure);
		
	}
}


/****************************************************************************
* 名    称：DS18B20_IO_OUT
* 功    能：输出
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void DS18B20_IO_OUT(u8 channel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(channel==1)
	{
    GPIO_InitStructure.GPIO_Pin=IO_DS18B20_1;
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	  GPIO_Init(GPIO_DS18B20_1,&GPIO_InitStructure);
	}else
	{
		GPIO_InitStructure.GPIO_Pin=IO_DS18B20_2;
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	  GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStructure);
	}
}


/****************************************************************************
* 名    称：void DS18B20_Reset(void)
* 功    能：复位
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void DS18B20_Reset(u8 channel)	   
{ 
  if(channel==1)
	{		
	  DS18B20_IO_OUT(channel);       //SET OUTPUT
    DS18B20_1_DQ_Low;
    delay_us(750);                 //拉低750us
    DS18B20_1_DQ_High;
	  delay_us(15);                  //15US
	}else
	{
		DS18B20_IO_OUT(channel);       //SET OUTPUT
    DS18B20_2_DQ_Low;
    delay_us(750);                 //拉低750us
    DS18B20_2_DQ_High;
	  delay_us(15);                  //15US
	}
}


/****************************************************************************
* 名    称：u8 DS18B20_Check(void)
* 功    能：温度传感器检测
* 入口参数：
* 出口参数：返回1:不存在 , 返回0:存在    	
* 说    明：初始化DS18B20的IO口 DQ 同时检测DS的存在
* 调用方法：
****************************************************************************/
u8 DS18B20_Check(u8 channel) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN(channel); 
	
	if(channel==1)
	{
		while(DS18B20_1_DQ_IN&&retry<200)//等待DS180回传0
		{
			retry++;
			delay_us(1);
		}
		
		if(retry>=200)
		{
			return 1;//超时返回
		}
		else 
		{
			retry=0;
		}
		
		while (!DS18B20_1_DQ_IN&&retry<240)//等待DS180回传1
		{
			retry++;
			delay_us(1);
		}
		
		if(retry>=240)
		{
			return 1;	//超时返回
		}
		else
		{
			return 0;
		}    
		
	}
	else
	{
		while(DS18B20_2_DQ_IN&&retry<200)//等待DS180回传0
		{
			retry++;
			delay_us(1);
		} 
		
		if(retry>=200)
		{
			return 1;//超时返回
		}
		else 
		{
			retry=0;
		}
		
		while (!DS18B20_2_DQ_IN&&retry<240)//等待DS180回传1
		{
			retry++;
			delay_us(1);
		}
		
		if(retry>=240)
		{
			return 1;	//超时返回
		}
		else
		{
			return 0;
		} 
	}
}


/****************************************************************************
* 名    称：u8 DS18B20_Configuration(void)
* 功    能：DS18B20初始化
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
u8 DS18B20_Configuration(u8 channel)
{
  u8 retry=0;

	if(channel==1)
	{
	   while(DS18B20_Init(1)&&retry<10)
	   {
		   retry += 1;
		   delay_ms(50);
 		   printf("18B20_1初始化失败\r\n");
	   }
	   if(retry >= 10)
	   {
		   return 1;
	   }
  }else
	{
		while(DS18B20_Init(2)&&retry<10)
	   {
		   retry += 1;
		   delay_ms(50);
 		   printf("18B20_2初始化失败\r\n");
	   }
	   if(retry >= 10)
	   {
		   return 1;
	   }
	}
}


/****************************************************************************
* 名    称：static void DS18B20_Read_Bit(void)
* 功    能：读一个bit
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
static u8 DS18B20_Read_Bit(u8 channel) 			 // read one bit
{
	u8 data;
	
	if(channel==1)
	{
		DS18B20_IO_OUT(channel);                
		DS18B20_1_DQ_Low;
		delay_us(2);
		DS18B20_IO_IN(channel);                  
		if(DS18B20_1_DQ_IN)
			data=1;
		else
			data=0;	 
		delay_us(50);           
		return data;
  }else
	{
		DS18B20_IO_OUT(channel);                
		DS18B20_2_DQ_Low;
		delay_us(2);
		DS18B20_IO_IN(channel);                  
		if(DS18B20_2_DQ_IN)
			data=1;
		else
			data=0;	 
		delay_us(50);           
		return data;
	}
}


/****************************************************************************
* 名    称：void DS18B20_Read_Byte(void)
* 功    能：读一个字节
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
u8 DS18B20_Read_Byte(u8 channel)    // read one byte
{
	u8 i,j,dat;
  dat=0;
	for (i=1;i<=8;i++) 
	{
		j=DS18B20_Read_Bit(channel);
    dat=(j<<7)|(dat>>1);
  }						    
  return dat;
}


/****************************************************************************
* 名    称：void DS18B20_Write_Byte(void)
* 功    能：写一个字节
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void DS18B20_Write_Byte(u8 dat,u8 channel)     
{             
	u8 j;
  u8 testb;
	
	if(channel==1)
	{
		DS18B20_IO_OUT(channel);                              
		for (j=1;j<=8;j++) 
		{
			testb=dat&0x01;
			dat=dat>>1;
			if (testb) 
			{
				DS18B20_1_DQ_Low;
				delay_us(2);                           
				DS18B20_1_DQ_High;
				delay_us(60);              
			}else 
			{
				DS18B20_1_DQ_Low;
				delay_us(60);             
				DS18B20_1_DQ_High;
				delay_us(2);                          
			}
		}
	}else
	{
		DS18B20_IO_OUT(channel);                              
		for (j=1;j<=8;j++) 
		{
			testb=dat&0x01;
			dat=dat>>1;
			if (testb) 
			{
				DS18B20_2_DQ_Low;
				delay_us(2);                           
				DS18B20_2_DQ_High;
				delay_us(60);              
			}else 
			{
				DS18B20_2_DQ_Low;
				delay_us(60);             
				DS18B20_2_DQ_High;
				delay_us(2);                          
			}
		}
		
	}
}



/****************************************************************************
* 名    称：u16 DS18B20_Get_wd(void)
* 功    能：读取温度值
* 入口参数：
* 出口参数：
* 说    明：适用于只有一个DS18B20
* 调用方法：
****************************************************************************/
u16 DS18B20_Get_wd(u8 channel)
{
		u16 temp=0;
		u8 TL,TH;


		DS18B20_Reset(channel);	   
		DS18B20_Check(channel);	 
		DS18B20_Write_Byte(0xcc,channel);
		DS18B20_Write_Byte(0x44,channel);

		delay_us(100);    
	
		DS18B20_Reset(channel);
		DS18B20_Check(channel);	 
		DS18B20_Write_Byte(0xcc,channel);
		DS18B20_Write_Byte(0xbe,channel);    
	
	
		TL=DS18B20_Read_Byte(channel); // LSB   
		TH=DS18B20_Read_Byte(channel); // MSB  


		temp=TH;
		temp=(temp<<8)+TL;
		return temp;


}




/****************************************************************************
* 名    称：u16 DS18B20_Configuration(u16 temp)
* 功    能：温度转换
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
u16 Temperature_convert(u16 temp)
{
	//u16 result = 0;
	u8 sign =0;
	u16 result=0;
	float tmp = 0;
	
	if((temp&0xF800)==0xF800)//负温度判断
	{
		temp=~temp;
		temp=temp+1;
		sign = 1;//负数标志
	}
	else
	{
		tmp = (float)(temp & 0x0f)*0.0625;
		result = 10*(((temp&0x7ff)>>4) + tmp);

	}
	
	return result;
}
