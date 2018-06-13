/****************************************************************************
* 文件名称：ad.c
* 说    明：AD采集文件
* 功    能：里面主要放AD采集涉及到的动作函数
* 内置方法1：void ADC_Configuration(void)	        AD初始化
* 内置方法2：void ADC_Configuration_Mode1(void)   AD初始化模式1
* 内置方法3：void ADC_Configuration_Mode2(void)   AD初始化模式2
* 内置方法4：void DMA_Configuration(void)	        DMA初始化
* 内置方法5：void AD_Sample_Mode1(u8 channel_index)  AD单通道采集
* 内置方法6：void AD_Sample_Mode2(void)              AD多通道采集
* 内置方法7：void Printf_AD_Value(void)           输出AD采集的结果
* 内置方法8：void Clear_AD_Value(void)            清除AD缓存 
* 内置方法9：
* 内置方法10：
* 内置方法11：
****************************************************************************/
#include "ad.h"


//-----------------------------------ADC1涉及到的变量--------------------------------------------
#define N 8  //为8个通道
#define M 10 //每通道采10次
// #define M 20 //每通道采20次
#define L 12 //为12列

u8 AD_MODE = 3;// 0 ：单通道模式，  1：多通道模式 ,

//单通道DMA方式
//8通道12列，每个占2字节 ----总共需要 192字节
vu16 AD_Value1[L][N][M]; //用来存放ADC转换结果，也是DMA的目标地址

//多通道DMA方式
vu16 AD_Value2[L][M][N]; //用来存放ADC转换结果，也是DMA的目标地址

//12列8行单孔
vu16 After_filter[L][N]; //用来存放求平均值之后的结果


//12列8行单孔
u16 AD_Average[12][8]={0}; //存放平均值

// float AD_K[8]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}; //存放8通道校正值
// u16 AD_K_Wave_channel[4][8]={{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}}; //存放8通道校正值

//步进速度  高：0  中：1  低：3
//步进脉冲  高：20  中：50  低：100
u8 G_Step_Mode_Plus = 20;//步进脉冲
/***************************************************************************
* 名    称：void ADC_Configuration(u8 index)
* 功    能：AD初始化
* 入口参数1：初始化模式选择
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void ADC_Configuration(u8 index)
{
	if(index == 0)
	{
		ADC_Configuration_Mode1();
	}
	else if(index == 1)
	{
		ADC_Configuration_Mode2();
	}

}
/****************************************************************************
* 名    称：void ADC_Configuration_Mode1(void)
* 功    能：AD初始化
* 入口参数：无
* 出口参数：无
* 说    明：单通道，连续转换
* 调用方法：
****************************************************************************/
void ADC_Configuration_Mode1(void)
{
	ADC_InitTypeDef ADC_InitStructure;

 	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立工作模式ADC_Mode_RegSimult
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//单通道模式
 	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//连续转换模式
//  	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//单次转换
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//外部触发关闭
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel=1;//通道数为1个采集通道

	ADC_Init(ADC1,&ADC_InitStructure);
  //1个AD通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	//使能DMA支持
 	ADC_DMACmd(ADC1,ENABLE);//重要
	//使能ADC1
	ADC_Cmd(ADC1,ENABLE);
	//ADC校准， 下面4句固定
	ADC_ResetCalibration(ADC1); //复位指定的ADC1的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//获取ADC1复位校准寄存器的状态,设置状态则等待
	ADC_StartCalibration(ADC1);//开始指定ADC1的校准状态
	while(ADC_GetCalibrationStatus(ADC1));//获取指定ADC1的校准程序,设置状态则等待

// 	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//启动AD转换
}

/****************************************************************************
* 名    称：void ADC_Configuration_Mode2(void)
* 功    能：AD初始化
* 入口参数：无
* 出口参数：无
* 说    明：多通道，连续转换， 通道数由 M 决定
* 调用方法：
****************************************************************************/
void ADC_Configuration_Mode2(void)
{
	ADC_InitTypeDef ADC_InitStructure;

 	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//独立工作模式ADC_Mode_RegSimult
 	ADC_InitStructure.ADC_ScanConvMode=ENABLE;//扫描模式
 	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//外部触发关闭
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel=N;//通道数为8个采集通道
	ADC_Init(ADC1,&ADC_InitStructure);
  //8个AD通道
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_239Cycles5);
 	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,5,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,6,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,7,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,8,ADC_SampleTime_239Cycles5);
	//使能DMA支持
 	ADC_DMACmd(ADC1,ENABLE);//重要
	//使能ADC1
	ADC_Cmd(ADC1,ENABLE);
	//ADC校准， 下面4句固定
	ADC_ResetCalibration(ADC1); //复位指定的ADC1的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//获取ADC1复位校准寄存器的状态,设置状态则等待
	ADC_StartCalibration(ADC1);//开始指定ADC1的校准状态
	while(ADC_GetCalibrationStatus(ADC1));//获取指定ADC1的校准程序,设置状态则等待

// 	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//启动AD转换
}

/****************************************************************************
* 名    称：void DMA_Configuration(u8 mode)
* 功    能：DMA初始化
* 入口参数：无
* 出口参数：无
* 说    明：DMA传输3要素: 数据源 ， 目的， 长度
* 调用方法：
****************************************************************************/
void DMA_Configuration(u8 mode)
{
	u32 dstaddr=0;
	if(mode == 0)//单通道连续采样模式
	{
		dstaddr = (u32)AD_Value1[0][0];
		DMA_Configuration_Mode1(dstaddr);
	}
	else if(mode == 1)//多通道连续采样模式
	{
		dstaddr = (u32)AD_Value2[0];
		DMA_Configuration_Mode2(dstaddr);
	}
}

/****************************************************************************
* 名    称：void DMA_Configuration_Mode1(u32 dst_addr)
* 功    能：DMA初始化
* 入口参数：无
* 出口参数：无
* 说    明：DMA传输3要素: 数据源 ， 目的， 长度
* 调用方法：
****************************************************************************/
void DMA_Configuration_Mode1(u32 dst_addr)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);//初始化通道一为默认值， AD1 使用DMA通道1

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//数据源
	DMA_InitStructure.DMA_MemoryBaseAddr = dst_addr;//目的

	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//外设作为数据来源
	DMA_InitStructure.DMA_BufferSize= M;//DMA缓存大小
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//内存寄存器地址递增
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//外设数据源宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//内存数据目的宽度 16位

 	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//工作正常模式
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//此DMA通道为高优先级
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMA禁止内存到内存传输
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
// 	DMA_Cmd(DMA1_Channel1,ENABLE);//启动DMA
}

/****************************************************************************
* 名    称：void DMA_Configuration_Mode2(u32 dst_addr)
* 功    能：DMA初始化
* 入口参数：无
* 出口参数：无
* 说    明：DMA传输3要素: 数据源 ， 目的， 长度
* 调用方法：
****************************************************************************/
void DMA_Configuration_Mode2(u32 dst_addr)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);//初始化通道一为默认值， AD1 使用DMA通道1

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//数据源
	DMA_InitStructure.DMA_MemoryBaseAddr = dst_addr;//目的

	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//外设作为数据来源
	DMA_InitStructure.DMA_BufferSize=M*N;//DMA缓存大小
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//内存寄存器地址递增
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//外设数据源宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//内存数据目的宽度 16位

 	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//工作正常模式
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//此DMA通道为高优先级
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMA禁止内存到内存传输
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
// 	DMA_Cmd(DMA1_Channel1,ENABLE);//启动DMA
}



/****************************************************************************
* 名    称：u16 AD_Sample_Mode0(u8 channel_index)
* 功    能：AD采样模式1
* 入口参数1：第几通道
* 出口参数：无
* 说    明：使用单通道，单次转换，不使用DMA
* 调用方法：
****************************************************************************/

u16 AD_Sample_Mode0(u8 channel_index)
{
	u8 index = channel_index;
 	ADC_RegularChannelConfig(ADC1,index,1,ADC_SampleTime_239Cycles5);//设置规则通道
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//启动AD转换
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}

/****************************************************************************
* 名    称：void AD_Sample_Mode1(u8 colum)
* 功    能：AD采样模式1
* 入口参数1：第几列
* 出口参数：无
* 说    明：使用单通道DMA方式， 一个通道连续转换N次
* 调用方法：
****************************************************************************/
void AD_Sample_Mode1(u8 colum)
{
	u8 i = 0;
	u32 dstaddr = 0;
	for(i=0;i<N;i++)
	{
		dstaddr =(u32)AD_Value1[colum][i];
		DMA_Configuration_Mode1(dstaddr);//DMA配置
		ADC_RegularChannelConfig(ADC1,i,1,ADC_SampleTime_239Cycles5);//设置规则通道
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);//启动AD转换
		DMA_Cmd(DMA1_Channel1,ENABLE);//启动DMA
		while(1)
		{
			if(DMA_GetFlagStatus(DMA1_FLAG_TC1)!=RESET)//等待DMA传输完成
			{
				DMA_ClearFlag(DMA1_FLAG_TC1);
				break;
			}
		}
	}
}

/****************************************************************************
* 名    称：void AD_Sample_Mode2(u8 colum)
* 功    能：AD采样模式2
* 入口参数1：colum 表示 第几列
* 出口参数：无
* 说    明：使用多通道DMA方式， 多个通道一次连续转换，总共重复N次，通道总数由DMA固定
* 调用方法：测试发现 01234567通道对应 数组下标为12345670
****************************************************************************/
void AD_Sample_Mode2(u8 colum)
{
	u32 dstaddr = 0;
	dstaddr =(u32)AD_Value2[colum];
	ADC_Configuration_Mode2();
 	DMA_Configuration_Mode2(dstaddr);//DMA配置， 每次使用都要从新配置一下，否则会出现DMA只进行一次	DMA传输
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//启动AD转换
	DMA_Cmd(DMA1_Channel1,ENABLE);//启动DMA
	while(1)
	{
		if(DMA_GetFlagStatus(DMA1_FLAG_TC1)!=RESET)//等待DMA传输完成
		{
			DMA_ClearFlag(DMA1_FLAG_TC1);
			break;
		}
	}
}


/****************************************************************************
* 名    称：void Process_AD_Value(u8 mode)
* 功    能：处理采集到的数据
* 入口参数：无
* 出口参数：无
* 说    明：去掉最大值10个最大值和10个最小值，然后求剩下的平均值
* 调用方法：
****************************************************************************/
void Process_AD_Value(u8 mode)
{
  u16 temp =0;
	u32 sum = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 l = 0;
	
	if(mode ==0)//单通道
	{
		for(k=0;k<L;k++)//12列
		{
			for(l=0;l<N;l++)//8通道
			{
				sum = 0;
				//排序
				for(i=0;i<M-1;i++)
				{
					for(j=0;j<M-i-1;j++)
					{
						if(AD_Value1[k][l][j]>AD_Value1[k][l][j+1])
						{
							temp = AD_Value1[k][l][j];
							AD_Value1[k][l][j] = AD_Value1[k][l][j+1];
							AD_Value1[k][l][j+1] = temp;
						}
					}
				}
				//去掉前后各5个数求平均
				for(i=2;i<M-2;i++)
				{
					sum += AD_Value1[k][l][i];
				}
				AD_Average[k][l] = sum/(M-4);
			}
		}
	}
	else if(mode ==1)//多通道
	{
		for(k=0;k<L;k++)//12列
		{
			for(l=0;l<N;l++)//8通道
			{
				sum = 0;
				//排序
				for(i=0;i<M-1;i++)
				{
					for(j=0;j<M-i-1;j++)
					{
						if(AD_Value2[k][j][l]>AD_Value2[k][j+1][l])
						{
							temp = AD_Value1[k][j][l];
							AD_Value2[k][j][l] = AD_Value2[k][j+1][l];
							AD_Value2[k][j+1][l] = temp;
						}
					}
				}
				//去掉前后各5个数求平均
				for(i=2;i<M-2;i++)
				{
					sum += AD_Value1[k][i][l];
				}
				AD_Average[k][l] = sum/(M-4);
			}
		}
	}


}

/****************************************************************************
* 名    称：void Printf_AD_Value_Average(void)
* 功    能：输出采集到的数据 的平均值
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void Printf_AD_Value_Average(void)
{
	u8 i=0;
	u8 j=0;
	int volt;
	
	for(i=0;i<L;i++)
	{
		for(j=0;j<N;j++)
		{
			volt = AD_Average[i][j]*3300/4096;
			printf("AD_Average[%d][%d] = %d ------ %d mv\r\n",i,j,AD_Average[i][j],volt);
		}
	}
}

/****************************************************************************
* 名    称：void Printf_AD_Value(u8 mode)
* 功    能：串口输出采集到的数据
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void Printf_AD_Value(u8 mode)
{
	int i,j,k;
	int volt;
	
	printf("---- AD采集 12列 8通道 数据 --3.5---%f-\r\n",3.1415);			
	for(i=0;i<L;i++)
	{
		printf("------第 %d 列数据--------------\r\n",i);
		for(j=0;j<N;j++)
		{
			printf("------------------第 %d 通道数据--------------\r\n",j);
			for(k=0;k<M;k++)
			{
				if(mode == 0)//单通道
				{
					volt = AD_Value1[i][j][k]*3300/4096;
					printf("AD_Value1[%d][%d][%d] = %d ------%dmv\r\n",i,j,k,AD_Value1[i][j][k],volt);
				}
				else if(mode == 1)//多通道
				{
					volt = AD_Value2[i][k][j]*3300/4096;
					printf("AD_Value2[%d][%d][%d] = %d ------%dmv\r\n",i,k,j,AD_Value2[i][k][j],volt);
				}
			  delay_ms(20);
			}
		}
	}
}

// void Printf_AD_Value(u8 mode)
// {
// 	int i,j,k;
// 	int volt;
// 	
// 	printf("---- AD采集 12列 8通道 数据 ----\r\n");			
// 	for(i=0;i<1;i++)
// 	{
// 		printf("------第 %d 列数据--------------\r\n",i);
// 		for(j=0;j<1;j++)
// 		{
// 			printf("------------------第 %d 通道数据--------------\r\n",j);
// 			for(k=0;k<M;k++)
// 			{
// 				if(mode == 0)//单通道
// 				{
// 					volt = AD_Value1[i][j][k]*3300/4096;
// 					printf("AD_Value1[%d][%d][%d] = %d ------%dmv\r\n",i,j,k,AD_Value1[i][j][k],volt);
// 				}
// 				else if(mode == 1)//多通道
// 				{
// 					volt = AD_Value2[i][k][j]*3300/4096;
// 					printf("AD_Value2[%d][%d][%d] = %d ------%dmv\r\n",i,k,j,AD_Value2[i][k][j],volt);
// 				}
// 			  delay_ms(20);
// 			}
// 		}
// 	}
// }

/****************************************************************************
* 名    称：void USB_Output_AD_Value(u8 mode)
* 功    能：USB端口输出采集到的数据
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void USB_Output_AD_Value(u8 mode)
{
	int i,j,k;
	int volt;
	
	printf("---- AD采集 12列 8通道 数据 --3.5---%f-\r\n",3.1415);			
	for(i=0;i<L;i++)
	{
		printf("------第 %d 列数据--------------\r\n",i);
		for(j=0;j<N;j++)
		{
			printf("------------------第 %d 通道数据--------------\r\n",j);
			for(k=0;k<M;k++)
			{
				if(mode == 0)//单通道
				{
					volt = AD_Value1[i][j][k]*3300/4096;
					printf("AD_Value1[%d][%d][%d] = %d ------%dmv\r\n",i,j,k,AD_Value1[i][j][k],volt);
				}
				else if(mode == 1)//多通道
				{
					volt = AD_Value2[i][k][j]*3300/4096;
					printf("AD_Value2[%d][%d][%d] = %d ------%dmv\r\n",i,k,j,AD_Value2[i][k][j],volt);
				}
			  delay_ms(20);
			}
		}
	}
}

/****************************************************************************
* 名    称：void Clear_AD_Value(u8 mode)
* 功    能：清空AD数据缓存
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void Clear_AD_Value(u8 mode)
{
	int i,j,k;
	for(i=0;i<L;i++)
	{
		for(j=0;j<N;j++)
		{
			for(k=0;k<M;k++)
			{
				if(mode == 0)//单通道
				{
					AD_Value1[i][j][k] = 0;
				}
				else if(mode == 1)//多通道
				{
					AD_Value2[i][k][j] = 0;
				}
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------
/****************************************************************************
* 名    称：void Adc_Init(void)
* 功    能：AD初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
// 	GPIO_InitTypeDef GPIO_InitStructure;
	
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
// 	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

//PA1 作为模拟通道输入引脚                         
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
/****************************************************************************
* 名    称：void Get_Adc(u8 ch)
* 功    能：获取通道值
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
u16 Get_Adc(u8 ch)   
{
  //设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

/****************************************************************************
* 名    称：void Get_Adc_Average(u8 ch,u16 times)
* 功    能：获取AD值
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
// u16 Get_Adc_Average(u8 ch,u16 times)
float Get_Adc_Average(u8 ch,u16 times)
{
// 	u32 temp_val=0;
	float temp_val=0;
// 	u16 ad_val[800]={0};
	u16 i=0;
	u16 j=0;
  u16 temp = 0;
  u16 count = times;
	//采集电压
	for(i=0;i<count;i++)
	{
// 		ad_val[i] = Get_Adc(ch);
		temp_val += Get_Adc(ch);
// 		delay_ms(5);
	}
	
	//排序
// 	for(i=0;i<count -1;i++)
// 	{
// 		for(j=0;j<count -i-1;j++)
// 		{
// 			if(ad_val[j]>ad_val[j+1])
// 			{
// 				temp = ad_val[j];
// 				ad_val[j] = ad_val[j+1];
// 				ad_val[j+1] = temp;
// 			}
// 		}
// 	}
		
// 	for(i=100;i<count-100;i++)
// 	for(i=0;i<count;i++)
// 	{
// 		temp_val += ad_val[i];
// 	}
	
	return temp_val/(count);
} 	 

/****************************************************************************
* 名    称：void Get_Adc_Mode0(u8 col,u8 speed)
* 功    能：获取AD值
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void Get_Adc_Mode0(u8 col,u8 speed)
{
	u16 result[200] = {0};
	u16 temp = 0;
	u32 sum = 0;
	u8 i =0;
	u8 j =0;
	u8 k =0;
	u8 mode =0;
	
	if(speed == 50)
	{
		mode = 10;
	}
	else if(speed == 100)
	{
		mode = 20;
	}
	else if(speed == 200)
	{
		mode = 40;
	}
	
	for(i=0;i<8;i++)
	{
		sum = 0;
		for(j=0;j<speed;j++)
		{
			result[j] = AD_Sample_Mode0(i);
		}
		
		//排序
		for(j=0;j<speed -1;j++)
		{
			for(k=0;k<speed -j-1;k++)
			{
				if(result[k]>result[k+1])
				{
					temp = result[k];
					result[k] = result[k+1];
					result[k+1] = temp;
				}
			}
		}
		//求均值
		for(j=mode;j<speed-mode;j++)
		{
			sum += result[j];
		}
		
		AD_Average[col][i] = sum/(speed - mode*2);
	}
}

