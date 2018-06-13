#include "pbdata.h"
#include "ds18b20.h"







/****************************************************************************
* ��    �ƣ�u8 DS18B20_Init(void)
* ��    �ܣ��¶ȴ�������ʼ��
* ��ڲ�����
* ���ڲ���������1:������ , ����0:����    	
* ˵    ������ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
* ���÷�����
****************************************************************************/
u8 DS18B20_Init(u8 channel)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	if(channel==1)
	{
 	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTG��ʱ�� 
	  GPIO_InitStructure.GPIO_Pin = IO_DS18B20_1;				    
 	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	  GPIO_Init(GPIO_DS18B20_1, &GPIO_InitStructure);
    GPIO_SetBits(GPIO_DS18B20_1,IO_DS18B20_1);    //���1
    DS18B20_Reset(1);
    return DS18B20_Check(1);
	}else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ�ܶ˿�D��ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);       //ӳ��PD01��ʹ��
		
	  GPIO_InitStructure.GPIO_Pin   = IO_DS18B20_2;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	  GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStructure);
	  //GPIO_ResetBits(GPIO_DS18B20_2,IO_DS18B20_2);    //���1
		//delay_ms(1000);
		GPIO_SetBits(GPIO_DS18B20_2,IO_DS18B20_2);    //���1
		DS18B20_Reset(2);
		return DS18B20_Check(2);
	}
}  



/****************************************************************************
* ��    �ƣ�DS18B20_IO_IN
* ��    �ܣ�����
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
* ��    �ƣ�DS18B20_IO_OUT
* ��    �ܣ����
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
* ��    �ƣ�void DS18B20_Reset(void)
* ��    �ܣ���λ
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void DS18B20_Reset(u8 channel)	   
{ 
  if(channel==1)
	{		
	  DS18B20_IO_OUT(channel);       //SET OUTPUT
    DS18B20_1_DQ_Low;
    delay_us(750);                 //����750us
    DS18B20_1_DQ_High;
	  delay_us(15);                  //15US
	}else
	{
		DS18B20_IO_OUT(channel);       //SET OUTPUT
    DS18B20_2_DQ_Low;
    delay_us(750);                 //����750us
    DS18B20_2_DQ_High;
	  delay_us(15);                  //15US
	}
}


/****************************************************************************
* ��    �ƣ�u8 DS18B20_Check(void)
* ��    �ܣ��¶ȴ��������
* ��ڲ�����
* ���ڲ���������1:������ , ����0:����    	
* ˵    ������ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
* ���÷�����
****************************************************************************/
u8 DS18B20_Check(u8 channel) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN(channel); 
	
	if(channel==1)
	{
		while(DS18B20_1_DQ_IN&&retry<200)//�ȴ�DS180�ش�0
		{
			retry++;
			delay_us(1);
		}
		
		if(retry>=200)
		{
			return 1;//��ʱ����
		}
		else 
		{
			retry=0;
		}
		
		while (!DS18B20_1_DQ_IN&&retry<240)//�ȴ�DS180�ش�1
		{
			retry++;
			delay_us(1);
		}
		
		if(retry>=240)
		{
			return 1;	//��ʱ����
		}
		else
		{
			return 0;
		}    
		
	}
	else
	{
		while(DS18B20_2_DQ_IN&&retry<200)//�ȴ�DS180�ش�0
		{
			retry++;
			delay_us(1);
		} 
		
		if(retry>=200)
		{
			return 1;//��ʱ����
		}
		else 
		{
			retry=0;
		}
		
		while (!DS18B20_2_DQ_IN&&retry<240)//�ȴ�DS180�ش�1
		{
			retry++;
			delay_us(1);
		}
		
		if(retry>=240)
		{
			return 1;	//��ʱ����
		}
		else
		{
			return 0;
		} 
	}
}


/****************************************************************************
* ��    �ƣ�u8 DS18B20_Configuration(void)
* ��    �ܣ�DS18B20��ʼ��
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
 		   printf("18B20_1��ʼ��ʧ��\r\n");
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
 		   printf("18B20_2��ʼ��ʧ��\r\n");
	   }
	   if(retry >= 10)
	   {
		   return 1;
	   }
	}
}


/****************************************************************************
* ��    �ƣ�static void DS18B20_Read_Bit(void)
* ��    �ܣ���һ��bit
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
* ��    �ƣ�void DS18B20_Read_Byte(void)
* ��    �ܣ���һ���ֽ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
* ��    �ƣ�void DS18B20_Write_Byte(void)
* ��    �ܣ�дһ���ֽ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
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
* ��    �ƣ�u16 DS18B20_Get_wd(void)
* ��    �ܣ���ȡ�¶�ֵ
* ��ڲ�����
* ���ڲ�����
* ˵    ����������ֻ��һ��DS18B20
* ���÷�����
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
* ��    �ƣ�u16 DS18B20_Configuration(u16 temp)
* ��    �ܣ��¶�ת��
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
u16 Temperature_convert(u16 temp)
{
	//u16 result = 0;
	u8 sign =0;
	u16 result=0;
	float tmp = 0;
	
	if((temp&0xF800)==0xF800)//���¶��ж�
	{
		temp=~temp;
		temp=temp+1;
		sign = 1;//������־
	}
	else
	{
		tmp = (float)(temp & 0x0f)*0.0625;
		result = 10*(((temp&0x7ff)>>4) + tmp);

	}
	
	return result;
}
