/****************************************************************************
* �ļ����ƣ�ad.c
* ˵    ����AD�ɼ��ļ�
* ��    �ܣ�������Ҫ��AD�ɼ��漰���Ķ�������
* ���÷���1��void ADC_Configuration(void)	        AD��ʼ��
* ���÷���2��void ADC_Configuration_Mode1(void)   AD��ʼ��ģʽ1
* ���÷���3��void ADC_Configuration_Mode2(void)   AD��ʼ��ģʽ2
* ���÷���4��void DMA_Configuration(void)	        DMA��ʼ��
* ���÷���5��void AD_Sample_Mode1(u8 channel_index)  AD��ͨ���ɼ�
* ���÷���6��void AD_Sample_Mode2(void)              AD��ͨ���ɼ�
* ���÷���7��void Printf_AD_Value(void)           ���AD�ɼ��Ľ��
* ���÷���8��void Clear_AD_Value(void)            ���AD���� 
* ���÷���9��
* ���÷���10��
* ���÷���11��
****************************************************************************/
#include "ad.h"


//-----------------------------------ADC1�漰���ı���--------------------------------------------
#define N 8  //Ϊ8��ͨ��
#define M 10 //ÿͨ����10��
// #define M 20 //ÿͨ����20��
#define L 12 //Ϊ12��

u8 AD_MODE = 3;// 0 ����ͨ��ģʽ��  1����ͨ��ģʽ ,

//��ͨ��DMA��ʽ
//8ͨ��12�У�ÿ��ռ2�ֽ� ----�ܹ���Ҫ 192�ֽ�
vu16 AD_Value1[L][N][M]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ

//��ͨ��DMA��ʽ
vu16 AD_Value2[L][M][N]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ

//12��8�е���
vu16 After_filter[L][N]; //���������ƽ��ֵ֮��Ľ��


//12��8�е���
u16 AD_Average[12][8]={0}; //���ƽ��ֵ

// float AD_K[8]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}; //���8ͨ��У��ֵ
// u16 AD_K_Wave_channel[4][8]={{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}}; //���8ͨ��У��ֵ

//�����ٶ�  �ߣ�0  �У�1  �ͣ�3
//��������  �ߣ�20  �У�50  �ͣ�100
u8 G_Step_Mode_Plus = 20;//��������
/***************************************************************************
* ��    �ƣ�void ADC_Configuration(u8 index)
* ��    �ܣ�AD��ʼ��
* ��ڲ���1����ʼ��ģʽѡ��
* ���ڲ�������
* ˵    ����
* ���÷�����
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
* ��    �ƣ�void ADC_Configuration_Mode1(void)
* ��    �ܣ�AD��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ͨ��������ת��
* ���÷�����
****************************************************************************/
void ADC_Configuration_Mode1(void)
{
	ADC_InitTypeDef ADC_InitStructure;

 	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//��������ģʽADC_Mode_RegSimult
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;//��ͨ��ģʽ
 	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//����ת��ģʽ
//  	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;//����ת��
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//�ⲿ�����ر�
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel=1;//ͨ����Ϊ1���ɼ�ͨ��

	ADC_Init(ADC1,&ADC_InitStructure);
  //1��ADͨ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	//ʹ��DMA֧��
 	ADC_DMACmd(ADC1,ENABLE);//��Ҫ
	//ʹ��ADC1
	ADC_Cmd(ADC1,ENABLE);
	//ADCУ׼�� ����4��̶�
	ADC_ResetCalibration(ADC1); //��λָ����ADC1��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�
	ADC_StartCalibration(ADC1);//��ʼָ��ADC1��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC1��У׼����,����״̬��ȴ�

// 	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����ADת��
}

/****************************************************************************
* ��    �ƣ�void ADC_Configuration_Mode2(void)
* ��    �ܣ�AD��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ͨ��������ת���� ͨ������ M ����
* ���÷�����
****************************************************************************/
void ADC_Configuration_Mode2(void)
{
	ADC_InitTypeDef ADC_InitStructure;

 	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//��������ģʽADC_Mode_RegSimult
 	ADC_InitStructure.ADC_ScanConvMode=ENABLE;//ɨ��ģʽ
 	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//�ⲿ�����ر�
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel=N;//ͨ����Ϊ8���ɼ�ͨ��
	ADC_Init(ADC1,&ADC_InitStructure);
  //8��ADͨ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_239Cycles5);
 	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,5,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,6,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,7,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_7,8,ADC_SampleTime_239Cycles5);
	//ʹ��DMA֧��
 	ADC_DMACmd(ADC1,ENABLE);//��Ҫ
	//ʹ��ADC1
	ADC_Cmd(ADC1,ENABLE);
	//ADCУ׼�� ����4��̶�
	ADC_ResetCalibration(ADC1); //��λָ����ADC1��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�
	ADC_StartCalibration(ADC1);//��ʼָ��ADC1��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC1��У׼����,����״̬��ȴ�

// 	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����ADת��
}

/****************************************************************************
* ��    �ƣ�void DMA_Configuration(u8 mode)
* ��    �ܣ�DMA��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����DMA����3Ҫ��: ����Դ �� Ŀ�ģ� ����
* ���÷�����
****************************************************************************/
void DMA_Configuration(u8 mode)
{
	u32 dstaddr=0;
	if(mode == 0)//��ͨ����������ģʽ
	{
		dstaddr = (u32)AD_Value1[0][0];
		DMA_Configuration_Mode1(dstaddr);
	}
	else if(mode == 1)//��ͨ����������ģʽ
	{
		dstaddr = (u32)AD_Value2[0];
		DMA_Configuration_Mode2(dstaddr);
	}
}

/****************************************************************************
* ��    �ƣ�void DMA_Configuration_Mode1(u32 dst_addr)
* ��    �ܣ�DMA��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����DMA����3Ҫ��: ����Դ �� Ŀ�ģ� ����
* ���÷�����
****************************************************************************/
void DMA_Configuration_Mode1(u32 dst_addr)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);//��ʼ��ͨ��һΪĬ��ֵ�� AD1 ʹ��DMAͨ��1

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//����Դ
	DMA_InitStructure.DMA_MemoryBaseAddr = dst_addr;//Ŀ��

	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//������Ϊ������Դ
	DMA_InitStructure.DMA_BufferSize= M;//DMA�����С
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//�ڴ�Ĵ�����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//��������Դ���Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//�ڴ�����Ŀ�Ŀ�� 16λ

 	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//��������ģʽ
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//��DMAͨ��Ϊ�����ȼ�
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMA��ֹ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
// 	DMA_Cmd(DMA1_Channel1,ENABLE);//����DMA
}

/****************************************************************************
* ��    �ƣ�void DMA_Configuration_Mode2(u32 dst_addr)
* ��    �ܣ�DMA��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����DMA����3Ҫ��: ����Դ �� Ŀ�ģ� ����
* ���÷�����
****************************************************************************/
void DMA_Configuration_Mode2(u32 dst_addr)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);//��ʼ��ͨ��һΪĬ��ֵ�� AD1 ʹ��DMAͨ��1

	DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//����Դ
	DMA_InitStructure.DMA_MemoryBaseAddr = dst_addr;//Ŀ��

	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//������Ϊ������Դ
	DMA_InitStructure.DMA_BufferSize=M*N;//DMA�����С
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//�ڴ�Ĵ�����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//��������Դ���Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//�ڴ�����Ŀ�Ŀ�� 16λ

 	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//��������ģʽ
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//��DMAͨ��Ϊ�����ȼ�
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMA��ֹ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
// 	DMA_Cmd(DMA1_Channel1,ENABLE);//����DMA
}



/****************************************************************************
* ��    �ƣ�u16 AD_Sample_Mode0(u8 channel_index)
* ��    �ܣ�AD����ģʽ1
* ��ڲ���1���ڼ�ͨ��
* ���ڲ�������
* ˵    ����ʹ�õ�ͨ��������ת������ʹ��DMA
* ���÷�����
****************************************************************************/

u16 AD_Sample_Mode0(u8 channel_index)
{
	u8 index = channel_index;
 	ADC_RegularChannelConfig(ADC1,index,1,ADC_SampleTime_239Cycles5);//���ù���ͨ��
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����ADת��
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}

/****************************************************************************
* ��    �ƣ�void AD_Sample_Mode1(u8 colum)
* ��    �ܣ�AD����ģʽ1
* ��ڲ���1���ڼ���
* ���ڲ�������
* ˵    ����ʹ�õ�ͨ��DMA��ʽ�� һ��ͨ������ת��N��
* ���÷�����
****************************************************************************/
void AD_Sample_Mode1(u8 colum)
{
	u8 i = 0;
	u32 dstaddr = 0;
	for(i=0;i<N;i++)
	{
		dstaddr =(u32)AD_Value1[colum][i];
		DMA_Configuration_Mode1(dstaddr);//DMA����
		ADC_RegularChannelConfig(ADC1,i,1,ADC_SampleTime_239Cycles5);//���ù���ͨ��
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����ADת��
		DMA_Cmd(DMA1_Channel1,ENABLE);//����DMA
		while(1)
		{
			if(DMA_GetFlagStatus(DMA1_FLAG_TC1)!=RESET)//�ȴ�DMA�������
			{
				DMA_ClearFlag(DMA1_FLAG_TC1);
				break;
			}
		}
	}
}

/****************************************************************************
* ��    �ƣ�void AD_Sample_Mode2(u8 colum)
* ��    �ܣ�AD����ģʽ2
* ��ڲ���1��colum ��ʾ �ڼ���
* ���ڲ�������
* ˵    ����ʹ�ö�ͨ��DMA��ʽ�� ���ͨ��һ������ת�����ܹ��ظ�N�Σ�ͨ��������DMA�̶�
* ���÷��������Է��� 01234567ͨ����Ӧ �����±�Ϊ12345670
****************************************************************************/
void AD_Sample_Mode2(u8 colum)
{
	u32 dstaddr = 0;
	dstaddr =(u32)AD_Value2[colum];
	ADC_Configuration_Mode2();
 	DMA_Configuration_Mode2(dstaddr);//DMA���ã� ÿ��ʹ�ö�Ҫ��������һ�£���������DMAֻ����һ��	DMA����
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����ADת��
	DMA_Cmd(DMA1_Channel1,ENABLE);//����DMA
	while(1)
	{
		if(DMA_GetFlagStatus(DMA1_FLAG_TC1)!=RESET)//�ȴ�DMA�������
		{
			DMA_ClearFlag(DMA1_FLAG_TC1);
			break;
		}
	}
}


/****************************************************************************
* ��    �ƣ�void Process_AD_Value(u8 mode)
* ��    �ܣ�����ɼ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����ȥ�����ֵ10�����ֵ��10����Сֵ��Ȼ����ʣ�µ�ƽ��ֵ
* ���÷�����
****************************************************************************/
void Process_AD_Value(u8 mode)
{
  u16 temp =0;
	u32 sum = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 l = 0;
	
	if(mode ==0)//��ͨ��
	{
		for(k=0;k<L;k++)//12��
		{
			for(l=0;l<N;l++)//8ͨ��
			{
				sum = 0;
				//����
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
				//ȥ��ǰ���5������ƽ��
				for(i=2;i<M-2;i++)
				{
					sum += AD_Value1[k][l][i];
				}
				AD_Average[k][l] = sum/(M-4);
			}
		}
	}
	else if(mode ==1)//��ͨ��
	{
		for(k=0;k<L;k++)//12��
		{
			for(l=0;l<N;l++)//8ͨ��
			{
				sum = 0;
				//����
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
				//ȥ��ǰ���5������ƽ��
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
* ��    �ƣ�void Printf_AD_Value_Average(void)
* ��    �ܣ�����ɼ��������� ��ƽ��ֵ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
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
* ��    �ƣ�void Printf_AD_Value(u8 mode)
* ��    �ܣ���������ɼ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void Printf_AD_Value(u8 mode)
{
	int i,j,k;
	int volt;
	
	printf("---- AD�ɼ� 12�� 8ͨ�� ���� --3.5---%f-\r\n",3.1415);			
	for(i=0;i<L;i++)
	{
		printf("------�� %d ������--------------\r\n",i);
		for(j=0;j<N;j++)
		{
			printf("------------------�� %d ͨ������--------------\r\n",j);
			for(k=0;k<M;k++)
			{
				if(mode == 0)//��ͨ��
				{
					volt = AD_Value1[i][j][k]*3300/4096;
					printf("AD_Value1[%d][%d][%d] = %d ------%dmv\r\n",i,j,k,AD_Value1[i][j][k],volt);
				}
				else if(mode == 1)//��ͨ��
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
// 	printf("---- AD�ɼ� 12�� 8ͨ�� ���� ----\r\n");			
// 	for(i=0;i<1;i++)
// 	{
// 		printf("------�� %d ������--------------\r\n",i);
// 		for(j=0;j<1;j++)
// 		{
// 			printf("------------------�� %d ͨ������--------------\r\n",j);
// 			for(k=0;k<M;k++)
// 			{
// 				if(mode == 0)//��ͨ��
// 				{
// 					volt = AD_Value1[i][j][k]*3300/4096;
// 					printf("AD_Value1[%d][%d][%d] = %d ------%dmv\r\n",i,j,k,AD_Value1[i][j][k],volt);
// 				}
// 				else if(mode == 1)//��ͨ��
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
* ��    �ƣ�void USB_Output_AD_Value(u8 mode)
* ��    �ܣ�USB�˿�����ɼ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void USB_Output_AD_Value(u8 mode)
{
	int i,j,k;
	int volt;
	
	printf("---- AD�ɼ� 12�� 8ͨ�� ���� --3.5---%f-\r\n",3.1415);			
	for(i=0;i<L;i++)
	{
		printf("------�� %d ������--------------\r\n",i);
		for(j=0;j<N;j++)
		{
			printf("------------------�� %d ͨ������--------------\r\n",j);
			for(k=0;k<M;k++)
			{
				if(mode == 0)//��ͨ��
				{
					volt = AD_Value1[i][j][k]*3300/4096;
					printf("AD_Value1[%d][%d][%d] = %d ------%dmv\r\n",i,j,k,AD_Value1[i][j][k],volt);
				}
				else if(mode == 1)//��ͨ��
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
* ��    �ƣ�void Clear_AD_Value(u8 mode)
* ��    �ܣ����AD���ݻ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
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
				if(mode == 0)//��ͨ��
				{
					AD_Value1[i][j][k] = 0;
				}
				else if(mode == 1)//��ͨ��
				{
					AD_Value2[i][k][j] = 0;
				}
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------
/****************************************************************************
* ��    �ƣ�void Adc_Init(void)
* ��    �ܣ�AD��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
// 	GPIO_InitTypeDef GPIO_InitStructure;
	
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
// 	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

//PA1 ��Ϊģ��ͨ����������                         
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  
//���ADCֵ
//ch:ͨ��ֵ 0~3
/****************************************************************************
* ��    �ƣ�void Get_Adc(u8 ch)
* ��    �ܣ���ȡͨ��ֵ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
u16 Get_Adc(u8 ch)   
{
  //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

/****************************************************************************
* ��    �ƣ�void Get_Adc_Average(u8 ch,u16 times)
* ��    �ܣ���ȡADֵ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
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
	//�ɼ���ѹ
	for(i=0;i<count;i++)
	{
// 		ad_val[i] = Get_Adc(ch);
		temp_val += Get_Adc(ch);
// 		delay_ms(5);
	}
	
	//����
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
* ��    �ƣ�void Get_Adc_Mode0(u8 col,u8 speed)
* ��    �ܣ���ȡADֵ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
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
		
		//����
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
		//���ֵ
		for(j=mode;j<speed-mode;j++)
		{
			sum += result[j];
		}
		
		AD_Average[col][i] = sum/(speed - mode*2);
	}
}

