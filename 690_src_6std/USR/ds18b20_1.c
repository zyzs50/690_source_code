#include "pbdata.h"
#include "ds18b20.h"


u8 G_Ds18b20_count = 0;//�������ϵ�DS18B20��Ŀ
u8 G_DS18B20_ID[4][8]={0};
u8 temp_dot_left = 0;//�¶���������
u8 temp_dot_right = 0;//�¶�С������


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
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTG��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = IO_DS18B20_1;				//PORTG.11 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIO_DS18B20_1, &GPIO_InitStructure);

	
	if(channel==1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTG��ʱ�� 
	  GPIO_InitStructure.GPIO_Pin = IO_DS18B20_1;				//PORTG.11 �������
 	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	  GPIO_Init(GPIO_DS18B20_1, &GPIO_InitStructure);
		
		GPIO_SetBits(GPIO_DS18B20_1,IO_DS18B20_1);    //���1
		DS18B20_Reset(1);
		return DS18B20_Check(1);
	}
  else
	{
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ�ܶ˿�D��ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);       //ӳ��PD01��ʹ��
	  GPIO_InitStructure.GPIO_Pin   = IO_DS18B20_2;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //����������50MHZ
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	   //�������
	  GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStructure);
	
		
		GPIO_SetBits(GPIO_DS18B20_2,IO_DS18B20_2);    //���1
		DS18B20_Reset(2);
		return DS18B20_Check(2);
  }  
}

/****************************************************************************
* ��    �ƣ�
* ��    �ܣ�����Ϊ����
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
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = IO_DS18B20_2; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /*��������*/
    GPIO_Init(GPIO_DS18B20_2, &GPIO_InitStructure);
	}
}
/****************************************************************************
* ��    �ƣ�
* ��    �ܣ�����Ϊ���
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
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = IO_DS18B20_2; 
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIO_DS18B20_2, &GPIO_InitStructure); 
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
	   DS18B20_IO_OUT(channel);//SET PA0 OUTPUT

	   DS18B20_1_DQ_Low;
	   delay_us(2);
		 DS18B20_1_DQ_High;
     DS18B20_IO_IN(channel);//SET PA0 INPUT
		 delay_us(12);
	   if(DS18B20_1_DQ_IN)
	    data=1;
	   else
	    data=0;	 
	   delay_us(45);           
      return data;
   }
	 else
	 {
		 DS18B20_IO_OUT(channel);//SET PA0 OUTPUT

	   DS18B20_2_DQ_Low;
	   delay_us(2);
		 DS18B20_2_DQ_High;
     DS18B20_IO_IN(channel);//SET PA0 INPUT
		 delay_us(12);
	   if(DS18B20_2_DQ_IN)
	    data=1;
	   else
	    data=0;	 
	   delay_us(50);           
      return data;
	 }
}

/****************************************************************************
* ��    �ƣ�static void DS18B20_Read_2Bit(void)
* ��    �ܣ���2��bit
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
// static u8 DS18B20_Read_2Bit(u8 channel) 			 // read one bit
// {
// 	u8 data=0;
// 	u8 i;
// 	
// 	if(channel==1)
// 	{
// 	  for(i=0;i<2;i++)
// 	   {
// 		   data = data << 1;
// 		   DS18B20_IO_OUT(channel);//SET PA0 OUTPUT
//  
// 		   DS18B20_1_DQ_Low;
// 		   delay_us(2);
//        DS18B20_1_DQ_High;
// 			 
// 		   DS18B20_1_IO_IN();//SET PA0 INPUT
//  		   delay_us(12);
// 		   if(DS18B20_1_DQ_IN)
// 		    {
// 			     data|=0x01;
// 		    }
// 		   delay_us(50);           
// 	   }
//     return data;
//   }
// 	else
// 	{
// 		 for(i=0;i<2;i++)
// 	   {
// 			 data = data << 1;
// 		   DS18B20_IO_OUT(channel);//SET PA0 OUTPUT
//  
// 		   DS18B20_2_DQ_Low;
// 		   delay_us(2);
//        DS18B20_2_DQ_High;
// 			 
// 		   DS18B20_2_IO_IN(channel);//SET PA0 INPUT
//  		   delay_us(12);
// 		   if(DS18B20_2_DQ_IN)
// 		    {
// 			     data|=0x01;
// 		    }
// 		   delay_us(50);           
// 	   }
//     return data;
// 	}
// 	
// }

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
    dat=(dat)|(j<<i);
  }						    
  return dat;
}

/****************************************************************************
* ��    �ƣ�static void DS18B20_Write_Bit(u8 dat)
* ��    �ܣ�дһ��bit
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
static void DS18B20_Write_Bit(u8 dat,u8 channel)     
{ 
  if(channel==1)	
	{
	   DS18B20_IO_OUT(channel);//SET PA0 OUTPUT;
	   if (dat) 
	   {

	     DS18B20_1_DQ_Low;
		   delay_us(2);                           
       DS18B20_1_DQ_High;
		   delay_us(60);              
	   }
	  else 
	  {
       DS18B20_1_DQ_Low;
		   delay_us(60);             
       DS18B20_1_DQ_High;
		   delay_us(2);                          
	   }
   }
	 else
	 {
		 	DS18B20_IO_OUT(channel);//SET PA0 OUTPUT;
	    if (dat) 
	    {

	      DS18B20_2_DQ_Low;
		    delay_us(2);                           
        DS18B20_2_DQ_High;
		    delay_us(60);              
	    }
	    else 
	    {
        DS18B20_2_DQ_Low;
		    delay_us(60);             
        DS18B20_2_DQ_High;
		    delay_us(2);                          
	   }
	 }
	 
	 
	 
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
	
	DS18B20_IO_OUT(channel);//SET PA0 OUTPUT;
	if(channel == 1)
	{	
    for (j=1;j<=8;j++) 
	  {
		   testb=dat&0x01;
       dat=dat>>1;
		   if(testb) 
		   {

			   DS18B20_1_DQ_Low;
         delay_us(2);                           
         DS18B20_1_DQ_High;
			   delay_us(60);              
       }
       else 
       {

			   DS18B20_1_DQ_Low;
			   delay_us(60);             
         DS18B20_1_DQ_High;
			   delay_us(2);                          
       }
     }
   }
	 else
	 {
		for (j=1;j<=8;j++) 
	  {
		   testb=dat&0x01;
       dat=dat>>1;
		   if(testb) 
		   {

			   DS18B20_2_DQ_Low;
         delay_us(2);                           
         DS18B20_2_DQ_High;
			   delay_us(60);              
       }
       else 
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
* ��    �ƣ�void DS18B20_Reset(void)
* ��    �ܣ���λ
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void DS18B20_Reset(u8 channel)	   
{                 
	DS18B20_IO_OUT(channel); //SET PA0 OUTPUT
  if(channel==1)
	{	
	  DS18B20_1_DQ_Low;
    delay_us(750);    //����750us
    DS18B20_1_DQ_High;
	  delay_us(15);     //15US
		
		
	}
	else
	{
		DS18B20_2_DQ_Low;
    delay_us(750);    //����750us
    DS18B20_2_DQ_High;
	  delay_us(15);     //15US
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
	
	if(channel ==1)
	{
	   
		 DS18B20_IO_IN(channel);
     while(DS18B20_1_DQ_IN&&retry<200)//�ȴ�DS180�ش�0
	   {
		   retry++;
		   delay_us(1);
	   };	 
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
	   };
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
		 DS18B20_IO_IN(channel);//SET PA0 INPUT	 
     while(DS18B20_2_DQ_IN&&retry<200)//�ȴ�DS180�ش�0
	   {
		   retry++;
		   delay_us(1);
	   };	 
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
	   };
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
* ��    �ƣ�void DS18B20_Match_Rom(u8 *rom)
* ��    �ܣ�ƥ��ROM
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
// u8 DS18B20_Match_Rom(u8 *rom)
// {
// 	u8 i;
//   DS18B20_Reset ();                 
//   DS18B20_Check();
// 	DS18B20_Write_Byte(0x55);
// 	for(i=0;i<8;i++)
// 	{
// 		DS18B20_Write_Byte( *(rom+i) );
// 	}
// 	return 1;
// }	
/****************************************************************************
* ��    �ƣ�u16 DS18B20_Get_Temp(void)
* ��    �ܣ���ȡ�¶�ֵ
* ��ڲ�����
* ���ڲ�����
* ˵    ���������ڶ�ȡ��� DS18B20
* ���÷�����
****************************************************************************/
u16 DS18B20_Get_Temp(u8 channel)
{
	u8 TL=0,TH=0;
	u16 temp=0;

  
  	DS18B20_Reset(channel);//��λ
	  DS18B20_Check(channel);
 	  //delay_us(10);     //15US
 	  DS18B20_Write_Byte(0xCC,channel); //����ROM����
    DS18B20_Write_Byte(0x44,channel); //�¶�ת������

    //delay_us(15);     //15US
	  DS18B20_Reset(channel);//��λ
	  DS18B20_Check(channel);
		//delay_us(10);     //15US
	  DS18B20_Write_Byte(0xCC,channel); //����ROM����
	  DS18B20_Write_Byte(0xBE,channel); //���¶�����

	  //delay_us(10);     //15US

	  TL=DS18B20_Read_Byte(channel);//LSB
	  TH=DS18B20_Read_Byte(channel);//MSB

	  temp=TH;
	  temp=(temp<<8)+TL;
	  return temp;
	

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
	DS18B20_Write_Byte(0xcc,channel);// skip rom
	DS18B20_Write_Byte(0x44,channel);// convert

	DS18B20_Reset(channel);
	DS18B20_Check(channel);	 
	DS18B20_Write_Byte(0xcc,channel);// skip rom
	DS18B20_Write_Byte(0xbe,channel);// read	    
	TL=DS18B20_Read_Byte(channel); // LSB   
	TH=DS18B20_Read_Byte(channel); // MSB  


	temp=TH;
	temp=(temp<<8)+TL;
	return temp;

}



/****************************************************************************
* ��    �ƣ�void DS18B20_Read_Rom(u8 *str,u8 len)
* ��    �ܣ��¶ȴ�������ROM
* ��ڲ�����
* ���ڲ���������1:������ , ����0:����    	
* ˵    ������ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
* ���÷�����
****************************************************************************/
// void DS18B20_Read_Rom(u8 *str,u8 len)
// {
// 	u8 i;	
// 	DS18B20_Reset();
// 	DS18B20_Check();	 
// 	DS18B20_Write_Byte(0x33);
// 	for(i=0;i<len;i++)
// 	{
// 		str[i]=DS18B20_Read_Byte();
// 	}
// }	

/****************************************************************************
* ��    �ƣ�u16 DS18B20_Get_wd(void)
* ��    �ܣ���ȡ�¶�ֵ
* ��ڲ�����
* ���ڲ�����
* ˵    ����������ֻ��һ��DS18B20
* ���÷�����
****************************************************************************/
// u16 DS18B20_Get_wd(void)
// {
// 	u16 temp=0;
// 	u8 TL,TH;
// 	DS18B20_Reset();	   
// 	DS18B20_Check();	 
// 	DS18B20_Write_Byte(0xcc);// skip rom
// 	DS18B20_Write_Byte(0x44);// convert

// 	DS18B20_Reset();
// 	DS18B20_Check();	 
// 	DS18B20_Write_Byte(0xcc);// skip rom
// 	DS18B20_Write_Byte(0xbe);// read	    
// 	TL=DS18B20_Read_Byte(); // LSB   
// 	TH=DS18B20_Read_Byte(); // MSB  


// 	temp=TH;
// 	temp=(temp<<8)+TL;
// 	return temp;

// }



/****************************************************************************
* ��    �ƣ�u8 DS18B20_Search_Rom_ID(void)
* ��    �ܣ�����ϵͳ�ϵ�DS18B20
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
// u8 DS18B20_Search_Rom_ID(void)
// {
// 	u8 k=0;
// 	u8 i=0;
// 	u8 collide=0;
// 	u8 m=0;
// 	u8 n=0;
// 	u8 num =0;
// 	u8 temp[5];
// 	u8 ss[64]={0};
// 	u8 tempp = 0;

// 	do
// 	{
// 		//��λ
// 		DS18B20_Reset();	   
// 		delay_us(480);
// 		//��������ROMָ��
// 		DS18B20_Write_Byte(0xf0);// search rom
// 		//
// 		for(m=0;m<8;m++)
// 		{
// 			u8 s = 0;
// 			for(n=0;n<8;n++)
// 			{
// 				k = DS18B20_Read_2Bit();//��2bit
// 				k = k &0x03;
// 				s = s>>1;
// 				if(k==0x01)
// 				{
// 					DS18B20_Write_Bit(0);
// 					ss[(m*8+n)]=0;
// 			  }
// 				else if(k==0x02)
// 				{
// 					s = s|0x80;
// 					DS18B20_Write_Bit(1);
// 					ss[(m*8+n)]=1;
// 				}
// 				else if(k == 0x00)
// 				{
// 					collide = m*8 + n +1;
// 					if(collide > temp[i])
// 					{
// 						DS18B20_Write_Bit(0);
// 						ss[(m*8+n)]=0;
// 						temp[++i] = collide;
// 					}
// 					else if(collide < temp[i])
// 					{
// 						s=s|((ss[(m*8+n)]&0x01)<<7); 
// 						DS18B20_Write_Bit(ss[(m*8+n)]);
// 					}
// 					else if(collide == temp[i])
// 					{
// 						s=s|0x80; 
// 						DS18B20_Write_Bit(1);
// 						ss[(m*8+n)]=1;
// 					  i = i-1;
// 					}
// 				}
// 				else if(k==0x03)
// 				{
// 					return 0;
// 				}
// 				delay_us(100); //ע�����ϵĲο�ʵ����û�������ʱ�����ϵͳ����ʱ���Ǻ�׼ȷ������ע����ʱ���ƣ��������ɨ��ROM Id��ʧ��         
// 			}
// 			tempp = s;
// 			G_DS18B20_ID[num][m]=tempp;
// 		}
// 		num++;
// 		G_Ds18b20_count = num;
// 	}
// 	while((temp[i]!=0)&&(num < 4));
// 	return 1;
// }
//
//

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
  }
	else
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
	//return 0;
}
/****************************************************************************
* ��    �ƣ�u16 Temperature_convert(u16 temp)
* ��    �ܣ��¶�ת��
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
float Temperature_convert(u16 temp)
{
	u16 tmp=0;
	float result = 0;
	u8 sign =0;
	
	if((temp&0xF800)==0xF800)//���¶��ж�
	{
// 		tmp=~temp;
// 		tmp=tmp+1;
// 		result = tmp*0.0625*10;
// 		result = result | 0x8000;

	}
	else
	{
		tmp = temp;
		result = (float)tmp*0.0625*10;
	}
		
	
	
	
// 	temp_dot_left  = temp>>4;
// 	result = temp_dot_left;
// 	temp_dot_right = ((temp&0x000f)*10)>>4;
// 	result = (result<<8) |temp_dot_right ;
// 	if(sign)
// 	{
// 	  result = result | 0x8000;
// 	}
	return result;
}



void ds18b20_return_TH_TL_CONF( u8 channel )  
{  
    char data,data_TH,data_TL,CONF; 

	
    //init_ds18b20();  
	   DS18B20_Init(channel);
	
    DS18B20_Write_Byte(0xcc,channel);       //����romָ��  
    DS18B20_Write_Byte(0xbe,channel);   //��ȡ�¶�ת��ֵ  
    data = DS18B20_Read_Byte(channel);  
    data = DS18B20_Read_Byte(channel);  
    data_TH = DS18B20_Read_Byte(channel);  
    data_TL = DS18B20_Read_Byte(channel);  
    CONF =DS18B20_Read_Byte(channel);  
    printf("���±������¶�Ϊ��%d��\r\n",data_TH);  
    printf("���±������¶�Ϊ��%d��\r\n",-(data_TL-128));  
    CONF &=0x60 ;  
    CONF =CONF>>5;  
    switch (CONF) {  
        case 0:  
            printf("ds18b20�Ĳ�������Ϊ9λ������Ϊ0.5��\r\n");  
            break;  
        case 1:  
            printf("ds18b20�Ĳ�������Ϊ10λ������Ϊ0.25��\r\n");  
            break;  
        case 2:  
            printf("ds18b20�Ĳ�������Ϊ11λ������Ϊ0.125��\r\n");  
            break;  
        case 3:  
            printf("ds18b20�Ĳ�������Ϊ12λ������Ϊ0.0625��\r\n");  
            break;  
        default:  
            printf("error!!\r\n");  
            break;  
    }  
} 
