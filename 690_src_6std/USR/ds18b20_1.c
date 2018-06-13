#include "pbdata.h"
#include "ds18b20.h"


u8 G_Ds18b20_count = 0;//单总线上的DS18B20数目
u8 G_DS18B20_ID[4][8]={0};
u8 temp_dot_left = 0;//温度整数部分
u8 temp_dot_right = 0;//温度小数部分


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
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PORTG口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = IO_DS18B20_1;				//PORTG.11 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIO_DS18B20_1, &GPIO_InitStructure);

	
	if(channel==1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PORTG口时钟 
	  GPIO_InitStructure.GPIO_Pin = IO_DS18B20_1;				//PORTG.11 推挽输出
 	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	  GPIO_Init(GPIO_DS18B20_1, &GPIO_InitStructure);
		
		GPIO_SetBits(GPIO_DS18B20_1,IO_DS18B20_1);    //输出1
		DS18B20_Reset(1);
		return DS18B20_Check(1);
	}
  else
	{
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能端口D的时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);       //映射PD01的使能
	  GPIO_InitStructure.GPIO_Pin   = IO_DS18B20_2;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //最高输出速率50MHZ
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	   //推挽输出
	  GPIO_Init(GPIO_DS18B20_2,&GPIO_InitStructure);
	
		
		GPIO_SetBits(GPIO_DS18B20_2,IO_DS18B20_2);    //输出1
		DS18B20_Reset(2);
		return DS18B20_Check(2);
  }  
}

/****************************************************************************
* 名    称：
* 功    能：设置为输入
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
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = IO_DS18B20_2; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /*浮空输入*/
    GPIO_Init(GPIO_DS18B20_2, &GPIO_InitStructure);
	}
}
/****************************************************************************
* 名    称：
* 功    能：设置为输出
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
* 名    称：static void DS18B20_Read_2Bit(void)
* 功    能：读2个bit
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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
    dat=(dat)|(j<<i);
  }						    
  return dat;
}

/****************************************************************************
* 名    称：static void DS18B20_Write_Bit(u8 dat)
* 功    能：写一个bit
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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
* 名    称：void DS18B20_Reset(void)
* 功    能：复位
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void DS18B20_Reset(u8 channel)	   
{                 
	DS18B20_IO_OUT(channel); //SET PA0 OUTPUT
  if(channel==1)
	{	
	  DS18B20_1_DQ_Low;
    delay_us(750);    //拉低750us
    DS18B20_1_DQ_High;
	  delay_us(15);     //15US
		
		
	}
	else
	{
		DS18B20_2_DQ_Low;
    delay_us(750);    //拉低750us
    DS18B20_2_DQ_High;
	  delay_us(15);     //15US
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
	
	if(channel ==1)
	{
	   
		 DS18B20_IO_IN(channel);
     while(DS18B20_1_DQ_IN&&retry<200)//等待DS180回传0
	   {
		   retry++;
		   delay_us(1);
	   };	 
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
	   };
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
		 DS18B20_IO_IN(channel);//SET PA0 INPUT	 
     while(DS18B20_2_DQ_IN&&retry<200)//等待DS180回传0
	   {
		   retry++;
		   delay_us(1);
	   };	 
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
	   };
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
* 名    称：void DS18B20_Match_Rom(u8 *rom)
* 功    能：匹配ROM
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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
* 名    称：u16 DS18B20_Get_Temp(void)
* 功    能：读取温度值
* 入口参数：
* 出口参数：
* 说    明：适用于读取多个 DS18B20
* 调用方法：
****************************************************************************/
u16 DS18B20_Get_Temp(u8 channel)
{
	u8 TL=0,TH=0;
	u16 temp=0;

  
  	DS18B20_Reset(channel);//复位
	  DS18B20_Check(channel);
 	  //delay_us(10);     //15US
 	  DS18B20_Write_Byte(0xCC,channel); //跳过ROM命令
    DS18B20_Write_Byte(0x44,channel); //温度转换命令

    //delay_us(15);     //15US
	  DS18B20_Reset(channel);//复位
	  DS18B20_Check(channel);
		//delay_us(10);     //15US
	  DS18B20_Write_Byte(0xCC,channel); //跳过ROM命令
	  DS18B20_Write_Byte(0xBE,channel); //读温度命令

	  //delay_us(10);     //15US

	  TL=DS18B20_Read_Byte(channel);//LSB
	  TH=DS18B20_Read_Byte(channel);//MSB

	  temp=TH;
	  temp=(temp<<8)+TL;
	  return temp;
	

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
* 名    称：void DS18B20_Read_Rom(u8 *str,u8 len)
* 功    能：温度传感器读ROM
* 入口参数：
* 出口参数：返回1:不存在 , 返回0:存在    	
* 说    明：初始化DS18B20的IO口 DQ 同时检测DS的存在
* 调用方法：
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
* 名    称：u16 DS18B20_Get_wd(void)
* 功    能：读取温度值
* 入口参数：
* 出口参数：
* 说    明：适用于只有一个DS18B20
* 调用方法：
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
* 名    称：u8 DS18B20_Search_Rom_ID(void)
* 功    能：遍历系统上的DS18B20
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
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
// 		//复位
// 		DS18B20_Reset();	   
// 		delay_us(480);
// 		//发送搜索ROM指令
// 		DS18B20_Write_Byte(0xf0);// search rom
// 		//
// 		for(m=0;m<8;m++)
// 		{
// 			u8 s = 0;
// 			for(n=0;n<8;n++)
// 			{
// 				k = DS18B20_Read_2Bit();//读2bit
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
// 				delay_us(100); //注意网上的参考实例都没有这个延时，如果系统的延时不是很准确，必须注意延时控制，少了这句扫描ROM Id会失败         
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
  }
	else
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
	//return 0;
}
/****************************************************************************
* 名    称：u16 Temperature_convert(u16 temp)
* 功    能：温度转换
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
float Temperature_convert(u16 temp)
{
	u16 tmp=0;
	float result = 0;
	u8 sign =0;
	
	if((temp&0xF800)==0xF800)//负温度判断
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
	
    DS18B20_Write_Byte(0xcc,channel);       //忽略rom指令  
    DS18B20_Write_Byte(0xbe,channel);   //读取温度转换值  
    data = DS18B20_Read_Byte(channel);  
    data = DS18B20_Read_Byte(channel);  
    data_TH = DS18B20_Read_Byte(channel);  
    data_TL = DS18B20_Read_Byte(channel);  
    CONF =DS18B20_Read_Byte(channel);  
    printf("过温报警的温度为：%d℃\r\n",data_TH);  
    printf("低温报警的温度为：%d℃\r\n",-(data_TL-128));  
    CONF &=0x60 ;  
    CONF =CONF>>5;  
    switch (CONF) {  
        case 0:  
            printf("ds18b20的测量精度为9位，精度为0.5℃\r\n");  
            break;  
        case 1:  
            printf("ds18b20的测量精度为10位，精度为0.25℃\r\n");  
            break;  
        case 2:  
            printf("ds18b20的测量精度为11位，精度为0.125℃\r\n");  
            break;  
        case 3:  
            printf("ds18b20的测量精度为12位，精度为0.0625℃\r\n");  
            break;  
        default:  
            printf("error!!\r\n");  
            break;  
    }  
} 
