#include "calibrate.h"
#include "ad.h"
#include "motor.h"
#include "math.h"					        //��ѧ������
#include "usb_config.h"

u8 SelectCalib = 2;//ѡ�����ֱ�׼���ʵİ�ֵ, �������Բ���ʱ��ѡ��A�ǹ����˵ģ�Ҫѡ��B

// u16 AD_Alllight[10][8]={0};		//�����Դֱ��ʱ8ͨ��ADֵ(10���߰干80��ADֵ),0:��ӦA ,7����ӦF
// u16 AD_Halflight[10][16]={0}; //�����м����ʱ16��
float AD_Alllight[10][8]={0};		//�����Դֱ��ʱ8ͨ��ADֵ(10���߰干80��ADֵ),0:��ӦA ,7����ӦF
float AD_Halflight[10][24]={0}; //�����м����ʱ16��: ��Ӧ6����׼����

float ABS_Array[10][24]={0.0};		  //����24��(2,4,6,8,10,12��ǰ4�׻�1,2��ȫ����)��ABSֵ�������һά��ʾ�߰��������ά��ʾ����������
float ABS_Aver_Array[24]={0.0};   	//����24��(2,4,6,8,10,12��ǰ4�׻�1,2��ȫ����)��ABSƽ��ֵ

float ABS_Array2[10][24]={0.0};		  //�������ֵ   ---����ǲ����õģ������������ռ䲻��������ȥ��
float ABS_Aver_Array2[24]={0.0};   	//�������ֵ   ---����ǲ����õģ������������ռ䲻��������ȥ��

float CV[24];				  //����Һ��������CVֵ
float SD[24];				  //����Һ��������SDֵ
float Accuration[24];		  // ��������׼ȷ��
float Differ[6];			  //����׼��ֵ
float Differ_betweenHoles[6];//����׼��ֵ
float ABS_Send[146];		  //����Ҫ���͵�96��ֵ

float Slope_405nm[8][6]={0};      //����405nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
float Slope_450nm[8][6]={0};	    //����450nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
float Slope_492nm[8][6]={0};	    //����492nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
float Slope_630nm[8][6]={0};	    //����630nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
float Slope_Ext01nm[8][6]={0};	    //��չ�˹�Ƭ01
float Slope_Ext02nm[8][6]={0};	    //��չ�˹�Ƭ02
float Slope_Ext03nm[8][6]={0};	    //��չ�˹�Ƭ03
float Slope_Ext04nm[8][6]={0};	    //��չ�˹�Ƭ04

float Intercept_405nm[8][6]={0};   //����405nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
float Intercept_450nm[8][6]={0};   //����450nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
float Intercept_492nm[8][6]={0};   //����492nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
float Intercept_630nm[8][6]={0};   //����630nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
float Intercept_Ext01nm[8][6]={0};	  //��չ�˹�Ƭ01
float Intercept_Ext02nm[8][6]={0};	  //��չ�˹�Ƭ02
float Intercept_Ext03nm[8][6]={0};	  //��չ�˹�Ƭ03
float Intercept_Ext04nm[8][6]={0};	  //��չ�˹�Ƭ04

float TestValue_405nm[8][5]={0};	 //����405nm������0.2A,0.5A��1.0A,1.5A,2.0A,��׼���ʵ�8ͨ������ֵ
float TestValue_450nm[8][5]={0};	 //����450nm������0.2A,0.5A��1.0A,1.5A,2.0A,��׼���ʵ�8ͨ������ֵ
float TestValue_492nm[8][5]={0};	 //����492nm������0.2A,0.5A��1.0A,1.5A,2.0A,��׼���ʵ�8ͨ������ֵ
float TestValue_630nm[8][5]={0};	 //����630nm������0.2A,0.5A��1.0A,1.5A,2.0A,��׼���ʵ�8ͨ������ֵ	
float TestValue_Ext01nm[8][5]={0};	  //��չ�˹�Ƭ01
float TestValue_Ext02nm[8][5]={0};	  //��չ�˹�Ƭ02
float TestValue_Ext03nm[8][5]={0};	  //��չ�˹�Ƭ03
float TestValue_Ext04nm[8][5]={0};	  //��չ�˹�Ƭ04

float StandardValue_405nm[7]  = {0,0.231,0.504,0.941,1.451,2.0,3.0};//{0,0.228,0.500,0.936,1.444};	   //405nm�����¹��������˹�Ƭ�İ�ֵ
float StandardValue_450nm[7]  = {0,0.218,0.472,0.867,1.341,2.0,3.0};//{0,0.217,0.471,0.864,1.336};		 //450nm�����¹��������˹�Ƭ�İ�ֵ
float StandardValue_492nm[7]  = {0,0.204,0.471,0.935,1.441,2.0,3.0};//{0,0.201,0.470,0.933,1.436};		 //4925nm�����¹��������˹�Ƭ�İ�ֵ
float StandardValue_630nm[7]  = {0,0.210,0.482,1.137,1.697,2.0,3.0};//{0,0.208,0.480,1.134,1.693};		 //630nm�����¹��������˹�Ƭ�İ�ֵ
float StandardValue_Ext01nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //��չ�˹�Ƭ01
float StandardValue_Ext02nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //��չ�˹�Ƭ02
float StandardValue_Ext03nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //��չ�˹�Ƭ03
float StandardValue_Ext04nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //��չ�˹�Ƭ04

/*֤���ţ�126300413*/
u8 targetValueA[96] = 
{
  0x02,0x28, 0x05,0x14, 0x09,0x41, 0x14,0x52, 0x20,0x00, 0x30,0x00, // 405
  0x02,0x13, 0x04,0x81, 0x09,0x67, 0x13,0x43, 0x20,0x00, 0x30,0x00, // 450
  0x01,0x99, 0x04,0x78, 0x09,0x34, 0x14,0x40, 0x20,0x00, 0x30,0x00, // 492
  0x02,0x06, 0x04,0x86, 0x11,0x36, 0x16,0x97, 0x20,0x00, 0x30,0x00,	// 630							   
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78, 0x20,0x00, 0x30,0x00,	// ��չ01
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78, 0x20,0x00, 0x30,0x00,	// ��չ02
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78, 0x20,0x00, 0x30,0x00,	// ��չ03
	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75, 0x20,0x00, 0x30,0x00  // ��չ04
};

/*֤���ţ�126300414*/
// u8 targetValueB[96] = 
// {														 
//   0x02,0x31, 0x05,0x14, 0x11,0x00, 0x14,0x46,  // 405
//   0x02,0x20, 0x04,0x86, 0x10,0x31, 0x13,0x39,  // 450
//   0x02,0x05, 0x04,0x99, 0x10,0x54, 0x14,0x37,  // 492
//   0x02,0x13, 0x05,0x67, 0x10,0x80, 0x16,0x95,	// 630							   
// 	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	// ��չ01
// 	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	// ��չ02
// 	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	// ��չ03
// 	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75   // ��չ04
// };
u8 targetValueB[96] = 
{														 
  0x02,0x28, 0x05,0x04, 0x10,0x91, 0x14,0x33, 0x20,0x00, 0x30,0x00,   // 405
  0x02,0x14, 0x04,0x74, 0x10,0x20, 0x13,0x28, 0x20,0x00, 0x30,0x00,  // 450
  0x01,0x99, 0x04,0x88, 0x10,0x42, 0x14,0x26, 0x20,0x00, 0x30,0x00,  // 492
  0x02,0x05, 0x05,0x56, 0x10,0x63, 0x16,0x83,	0x20,0x00, 0x30,0x00, // 630							   
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // ��չ01
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // ��չ02
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // ��չ03
	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75, 0x20,0x00, 0x30,0x00  // ��չ04
};

/*֤���ţ�126300413*/
u8 targetValueC[96] = 
{
  0x02,0x28, 0x05,0x14, 0x09,0x41, 0x14,0x52, 0x20,0x00, 0x30,0x00,  // 405
  0x02,0x13, 0x04,0x81, 0x09,0x67, 0x13,0x43, 0x20,0x00, 0x30,0x00,  // 450
  0x01,0x99, 0x04,0x78, 0x09,0x34, 0x14,0x40, 0x20,0x00, 0x30,0x00,  // 492
  0x02,0x06, 0x04,0x86, 0x11,0x36, 0x16,0x97,	0x20,0x00, 0x30,0x00,  // 630							   
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // ��չ01
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // ��չ02
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // ��չ03
	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75, 0x20,0x00, 0x30,0x00   // ��չ04
};

u8 Wavelength_Flag = 0; //����ʱ���Ƿ�ʹ��˫������ 1��ʹ��˫������ 0��ʹ�õ�����

// u16 AD_ALLlight_Single[8]={0};          //���浥�����¹�Դֱ��ʱ8ͨ��ADֵ
// u16 AD_ALLlight_Double[8]={0};	        //����β�����˫����ģʽ����Դֱ��ʱ8ͨ��ADֵ			  
float AD_ALLlight_Single[8]={0};          //���浥�����¹�Դֱ��ʱ8ͨ��ADֵ
float AD_ALLlight_Double[8]={0};	        //����β�����˫����ģʽ����Դֱ��ʱ8ͨ��ADֵ			  

// u16 AD_Halflight_Single[12][8]={0};     //���浥�������м����ʱ96��ADֵ��8��12�У�
// u16 AD_Halflight_Double[12][8]={0};     //����β�����˫����ģʽ�����м����ʱ96��ADֵ��8��12�У�
float AD_Halflight_Single[12][8]={0};     //���浥�������м����ʱ96��ADֵ��8��12�У�
float AD_Halflight_Double[12][8]={0};     //����β�����˫����ģʽ�����м����ʱ96��ADֵ��8��12�У�

float  ABS_SingleWave0[12][8]={0.0};	//δ��������ABS   
float  ABS_DoubleWave0[12][8]={0.0};	//δ��������ABS

float  ABS_SingleWave[12][8]={0.0};		    //���浥������96�׵�ABSֵ����ʾֵ��8��12�У�
float  ABS_DoubleWave[12][8]={0.0};		    //����β�����˫����ģʽ����96�׵�ABS��ʾֵ��8��12�У�


/****************************************************************************
* ��    �ƣ�void ADC0_Sample_ALLlight(u16 *address,u8 mode)
* ��    �ܣ�
* ��ڲ�����u16 *address  : ���ADֵ�ĵ�ַ
* ��ڲ�����u8 mode  :�ɼ�ģʽ , 0:ABCD �У� 1��EFGH�У� 2��ABCDEFGH ��
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void ADC0_Sample_Mode(float *address,u8 mode)
{
	u8 i=0;
  u8 count = 0;
	u8 channel = 0;
// 	u16 temp[8] = {0};
	//�ɼ���ѹ
	if(mode==0)//ABCD
	{
		channel = 7;
		count = 4;
	}
	else if(mode ==1)//EFGH
	{
		channel = 3;
		count = 4;
	}
	else if(mode ==2)//ABCDEFGH
	{
		channel = 7;
		count = 8;
	}

	for(i=0;i<count;i++)
	{		
		*address = Get_Adc_Average(channel,800);
// 		temp[i] = *address;
		address += 1; 
		channel -= 1;
// 		delay_ms(5);
	}
	
// 	for(i=0;i<count;i++)
// 	{
// 		printf("temp[%d]= %d\r\n",i,temp[i]);
// 	}
}

/****************************************************************************
* ��    �ƣ�u8 AD_Loop_Sample_K_B(u8 mode,u8 type)
* ��    �ܣ�
* ��ڲ�����u8 mode  :�ɼ�ģʽ , 0:ABCD �У� 1��EFGH�У� 2��ABCDEFGH ��
* ��ڲ�����u8 type  :0����·���ܼ���,  1��Һ����������
* ���ڲ�����
* ˵    �����κ������߰�������� ���Ա�׼����
* ���÷�����
****************************************************************************/
u8 AD_Loop_Sample_K_B(u8 mode,u8 type)
{
 	u8 Response_CMD[3]={0x68,0x00,0xff};
// 	u16 *address = 0;
	float *address = 0;
	u8 ret =0;
	u8 col =0;
	uint8_t loop = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint16_t k = 0;
	uint16_t pluse_Max = 600;
	uint8_t count = 100;
	
	//10���߰��ͨ��ADֵ
	for(loop=0;loop<10;loop++)
	{
		pluse_Max = 600;
		count     = 100;
		//ָ�򱣴�ADֱ��ʱ�ĵ�ַ
		address = &AD_Alllight[loop][0];

		//ø��帴λ
		ret = Plate_Motor_Reset(1);
		if(ret)//��λʧ��
		{
			for(k=0;k<200;k++)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
			}
			Response_CMD[1]=ret;
			USB_TxWrite(Response_CMD, 3);//������Ӧ
			return 1;
		}
		delay_ms(10);
		
		//�ɼ�8ͨ����Դֱ��ADֵ
		ADC0_Sample_Mode(address,2);
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);//ʹ�ܵ��
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);//�������
		
		//�ö�λƬ�������
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//���ס��1�� ͨ��Ϊ0
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
			//����������һ��
			for(k=0;k<200;k++)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
			}
			
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			Response_CMD[1]=count;
			USB_TxWrite(Response_CMD, 3);//������Ӧ
			return 2;//ͬ������λ����������
		}	
			
		address = &AD_Halflight[loop][0];
		
		if(type == 0)
		{
// 			col = 8;//��·���ܼ��� �� 8 �� 
			col = 12;//��·���ܼ��� �� 12 ��, 1,3,5,7,9,11�� �ֱ��� 0.2,0.5,1.0.1.5,2.0,3.0
		}
		else if(type == 1)
		{
			col = 2; //Һ�����������2��
		}
		//��ʽ��ʼ��λ
		for(i=0;i<col;i++)
		{
			//����7������ʽΪ���õ�Ƭ�ƶ�һ����룬�ö�λ��ƫ������������һ����λ��
			for(j=0;j<7;j++)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				pluse_Max = pluse_Max - 1;
			}
			
			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//���ס��1�� ͨ��Ϊ0
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				pluse_Max = pluse_Max - 1;
			}
			
			if(pluse_Max == 0)
			{
			  //����������һ��
				for(k=0;k<200;k++)
				{
					GPIO_SetBits(GPIOD,GPIO_Pin_2);
					delay_us(250);
					GPIO_ResetBits(GPIOD,GPIO_Pin_2);
					delay_us(250);
				}
				
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
				Response_CMD[1]=pluse_Max;
				USB_TxWrite(Response_CMD, 3);//������Ӧ
				return 3;//��λʧ��
			}

			if(type==0)
			{
				//1,3,5,7�д���˹�Ƭ��׼����
				if((i==1)||(i==3)||(i==5)||(i==7)||(i==9)||(i==11))
				{
					delay_ms(500);					
					ADC0_Sample_Mode(address,mode); //4 ͨ�� 4�� Ϊ 16������
					address = address +4;
				}
			}
			else if(type==1)
			{
				//0,1�д��Һ������
				ADC0_Sample_Mode(address,2);//�ɼ�8ͨ�� ��--8ͨ��2��Ϊ 16������
				address = address +8;
			}
		}
	}
	ret = 0;
	//ø��帴λ
	ret = Plate_Motor_Reset(1);
	if(ret)//��λʧ��
	{
		//����������һ��
		for(k=0;k<200;k++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
		
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 4;
	}
	delay_ms(10);

	return 0;
}


/****************************************************************************
* ��    �ƣ�void Data_Handling_Optical(u8 mode,u8 type)
* ��    �ܣ�����10���߰�õ��� 16 �� ABS ֵ
* ��ڲ�����mode   0��ABCDͨ�� ,     1��EFGHͨ��  
* ��ڲ�����type   0����·���ܼ���,  1��Һ����������
* ���ڲ�����
* ˵    ���� ��·���ܼ��飬Һ����������  ���ǵõ�16�����ݣ� ǰ���� 4��ͨ�� 4�У� ������8��ͨ��2��
* ���÷�����
****************************************************************************/
void Data_Handling_Optical(u8 mode,u8 type)
{
	u8 i=0;
	u8 j=0;
	u8 col=0;
	
	u16 light_in = 0;  //ֱ������
	u16 light_out = 0; //�ɼ�����
	float A_BS = 0.0;
	
	if(type == 0) //0����·���ܼ���,  
	{
		//��10���߰壬ÿ�� 24�׵�ABS
		if(mode == 0)//ABCD��
		{
			for(i=0;i<10;i++)//10���߰�
			{
				for(col=0;col<6;col++)//6��
				{
					for(j=0;j<4;j++)//4��
					{
						light_in  = AD_Alllight[i][j];
						light_out = AD_Halflight[i][j+col*4];
						A_BS      = (float)light_in / (float)light_out;     //����ǿ��תΪfloat�ͣ����򽫵��¼����������ȷ
						A_BS      = log10(A_BS);                 //���������
						ABS_Array[i][j+col*4] = A_BS;			    //����ABSֵ������ABS_Array[]��
					}
				}
			}
		}
		else if(mode == 1)//EFGH��
		{
			for(i=0;i<10;i++)//10���߰�
			{
				for(col=0;col<6;col++)//4��
				{
					for(j=0;j<4;j++)//4��
					{
						light_in  = AD_Alllight[i][j+4];
						light_out = AD_Halflight[i][j+col*4];
						A_BS      = (float)light_in / (float)light_out;     //����ǿ��תΪfloat�ͣ����򽫵��¼����������ȷ
						A_BS      = log10(A_BS);                 //���������
						ABS_Array[i][j+col*4] = A_BS;			    //����ABSֵ������ABS_Array[]��
					}
				}
			}
		}
	}
	else if(type == 1) //1��Һ����������
	{
			for(i=0;i<10;i++)//10���߰�
			{
				for(col=0;col<2;col++)//2��
				{
					for(j=0;j<8;j++)//8��
					{
						light_in  = AD_Alllight[i][j];
						light_out = AD_Halflight[i][j+col*8];
						A_BS      = (float)light_in / (float)light_out;     //����ǿ��תΪfloat�ͣ����򽫵��¼����������ȷ
						A_BS      = log10(A_BS);                 //���������
						ABS_Array[i][j+col*8] = A_BS;			    //����ABSֵ������ABS_Array[]��
					}
				}
			}
	}
	
	//��10���߰��ƽ��ABS
	for(i=0;i<24;i++)
	{
		A_BS = 0.0;
		for(j=0;j<10;j++)
		{
			A_BS += ABS_Array[j][i];
		}
		A_BS = A_BS/10;
		ABS_Aver_Array[i] = A_BS;
	}
	
}


/****************************************************************************
* ��    �ƣ�void Bcdvert(u8 sour[], float dest[], u8 wavel)
* ��    �ܣ�
* ��ڲ����������׼���ʵİ�ֵ
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
u8 Bcdvert(u8 sour[], float dest[], u8 wavel)			   //���յ�������ת��Ϊfloat��
{	  
  int tempdata,i;
	for(i=0; i<12; i++)
	{
		tempdata = sour[wavel*12 + i++];
		tempdata = (tempdata<<8) + sour[wavel*12 +i];
		dest[i/2 + 1] = (float)((tempdata>>12)&0x0f) + (float)((tempdata >> 8)&0x0f)/10 + (float)((tempdata >> 4)&0x0f)/100 + (float)((tempdata&0x0f))/1000;  
	}	
	return 0;
}


/****************************************************************************
* ��    �ƣ�void CV_SD_Function(float X[10][16],float x[16],u8 n)
* ��    �ܣ�
* ��ڲ�������CV��SDֵ��CV=SD/x��SDΪ��׼�xΪ��ֵ��
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void CV_SD_Function(float X[10][24],float x[24],u8 n)
{
	 u8 i,j;
	 u8 m = 0;
	 //float CV,SD;
	 float a = 0.0;
	 for(i = 0;i < 24;i ++)
	 {
	   for(j=0;j<n;j++)
	 	 {
		   a = a + (X[j][i] - x[i] ) * (X[j][i] - x[i] );			//���ۼӺ�
		 }
		 SD[m] = sqrt(a / (n - 1));								    //��SDֵ
	   CV[m] = SD[m] / x[i];										 //��CVֵ
		 m ++;
		 a = 0.0;	  
	 }
	 m = 0;
}

	
/****************************************************************************
* ��    �ƣ�void CV_SD_Function1(float X[16],float x[4],u8 n)
* ��    �ܣ�
* ��ڲ�������׼��
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void CV_SD_Function1(float X[24],float x[6],u8 n)
{
	 u8  i,j;
	 u8  m = 0;
	 //float CV,SD;
	 float a = 0.0;
	 float std;
	 i = 0;
	 for(j=0;j<24;j++)
	 {	     
	   a = a + (X[j] - x[i] ) * (X[j] - x[i] );
	 	 if(j == 3 || j == 7 || j == 11 || j == 15 || j == 19 || j == 23)
		 {
		 	 std = sqrt(a / (n - 1));
			 Differ_betweenHoles[m]	= std / x[i];				 //��׼��CVֵ
			 m ++;
			 i ++;
			 a = 0.0;
		 }
	 }
}

/****************************************************************************
* ��    �ƣ�void X_Y_Kb(u8 Wavelength_Main,u8 Number_Channel)
* ��    �ܣ�����С���˷���K��bֵ��ÿͨ��4��K��bֵ��������ABS��ʾֵ��������ABCD��EFGHͨ����׼ȷ�ȡ��ظ��ԡ�ͨ����ļ����
* ��ڲ�����Wavelength_Main ��ʹ�õĲ���
* ��ڲ�����Number_Channel  ��0:ABCD �У� 1��EFGH�У�
* ���ڲ�����
* ˵    ������·���ܼ��� ��������� K,Bֵ�� ��Һ���������� ���� K,B ֵ�����������
* ���÷�����
****************************************************************************/
void X_Y_Kb(u8 Wavelength_Main,u8 Number_Channel)
{
	u8 y = 0;
	u8 i = 0;		 										//��ʱ����
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;		 
	u8 n = 2;		 
	u8 h = 0;		 
	float a1 = 0;												 //�����������ABSƽ��ֵ�ۼӺ�
	float a2 = 0;												 //�����������ABSƽ��ֵ�ۼӺ�
	float c1 = 0;												 //�����������������¹��������˹�Ƭ��׼ֵ���ۼӺ�
	float c2 = 0;												 //�����������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
	float result = 0;											 //�ۼӺͱ���
	u8 Num_Test = 0;											 //�߰����
	u8 Num_Glass;												 //���������˹�Ƭ������������4�֣�0.2A,0.5A,1.0A,1.5A
	u8 Num_PerGlass;											 //ÿ�������˹�Ƭռ��λ������4�ף�

	if(SelectCalib == 0x01)											 //��ֵA
	{
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_405nm, 0);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_450nm, 1);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_492nm, 2);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_630nm, 3);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext01nm, 4);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext02nm, 5);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext03nm, 6);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext04nm, 7);
	}
	else if(SelectCalib == 0x02)										 //��ֵB
	{
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_405nm, 0);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_450nm, 1);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_492nm, 2);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_630nm, 3);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext01nm, 4);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext02nm, 5);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext03nm, 6);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext04nm, 7);
	}
	else																 //��ֵC
	{
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_405nm, 0);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_450nm, 1);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_492nm, 2);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_630nm, 3);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext01nm, 4);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext02nm, 5);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext03nm, 6);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext04nm, 7);
	}
	
	for(Num_Glass=0;Num_Glass<6;Num_Glass++)						 	 //����4���˹�Ƭ0.2 ��0.5��1.0��1.5, 2.0�� 3.0
	{
		for(Num_PerGlass=0;Num_PerGlass<8;Num_PerGlass++)			  //����ÿ��ͨ����6��K��bֵ
		{
			if(Num_PerGlass != 0)										  //������Ȳ�Ϊ0���ۼӺ�
			{
					a1 += ABS_Aver_Array[k];								  //�ۼ�ABSƽ��ֵ
					a2 += ABS_Aver_Array[k] * ABS_Aver_Array[k];			  //��ABSƽ��ֵ�ۼӺ�
			}
			else
			{
				 a1 = 0;												  //�������Ϊ0���ۼӺ�
				 a2 = 0;
			}
			if(Wavelength_Main == 1)                     // 405nm���� 
			{ 														  //��405nm�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c1 +=	StandardValue_405nm[i];
					c2 += ABS_Aver_Array[k] * StandardValue_405nm[i];	 //��405nm������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			else if(Wavelength_Main == 2)			                  //450nm����
			{ 
					c1 +=	StandardValue_450nm[i];									  //��450nm�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c2 += ABS_Aver_Array[k] * StandardValue_450nm[i];				  //��450nm������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			else if(Wavelength_Main == 3)                // 492nm���� 
			{ 
					c1 +=	StandardValue_492nm[i];									  //��492nm�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c2 += ABS_Aver_Array[k] * StandardValue_492nm[i];				  //��492nm������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			else if(Wavelength_Main == 4)				  //630nm����
			{ 
					c1 +=	StandardValue_630nm[i];									  //��630nm�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c2 += ABS_Aver_Array[k] * StandardValue_630nm[i];				  //��630nm������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			else if(Wavelength_Main == 5)				  //��չ����01
			{ 
					c1 +=	StandardValue_Ext01nm[i];									  //����չ����01�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c2 += ABS_Aver_Array[k] * StandardValue_Ext01nm[i];				  //����չ����01������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			else if(Wavelength_Main == 6)				  //��չ����02
			{ 
					c1 +=	StandardValue_Ext02nm[i];									  //����չ����02�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c2 += ABS_Aver_Array[k] * StandardValue_Ext02nm[i];				  //����չ����02������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			else if(Wavelength_Main == 7)				  //��չ����03
			{ 
					c1 +=	StandardValue_Ext03nm[i];									  //����չ����03�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c2 += ABS_Aver_Array[k] * StandardValue_Ext03nm[i];				  //����չ����03������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			else if(Wavelength_Main == 8)				  //��չ����04
			{ 
					c1 +=	StandardValue_Ext04nm[i];									  //����չ����04�����¹��������˹�Ƭ��׼ֵ���ۼӺ�
					c2 += ABS_Aver_Array[k] * StandardValue_Ext04nm[i];				  //����չ����04������ABSƽ��ֵ�͹��������˹�Ƭ��׼ֵ�˻����ۼӺ�
			}
			
			
			
			if(Num_PerGlass != 0)									  //��׼��������Ȳ�Ϊ0
			{
					 k += 4;											  
			}
			i++;
			m++;
			if( m == 2 || m == 4 || m == 6 || m == 8 || m == 10 || m == 12 )				  //�����������4��K��Bֵ
			{														  
				if(Wavelength_Main == 1)                                       //405nm����
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
							Slope_405nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_405nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
// 							if(h != 3)
							if(h != 5)
							{
								TestValue_405nm[j+4][h] = ABS_Aver_Array[y];		//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
							}
					 }
					 else if(Number_Channel == 0x00)														//ABCDͨ��
					 {
							Slope_405nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_405nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h!= 5)
							{
								TestValue_405nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A,1.5,2.0����ֵ��������TestValue_405nm[][]��
							}
					 }
					 y += 4;																				//ѡ��0.5A��1.0A�Ĳ���ƽ��ֵ
					 if(h == 5)
					 {																					
// 							y = y - 15;
							y = y - 23;
					 }	                                     	//һͨ��0.2A,0.5A,1.0A����ֵ����������һͨ��0.2A,0.5A,1.0A����ֵ�Ĵ�ȡ
				}
				else if(Wavelength_Main == 2)			                  //450nm����
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
							Slope_450nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_450nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h != 5)
							{
								TestValue_450nm[j+4][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��														
							}
					 }
					 else	if(Number_Channel == 0x00)																	//ABCEͨ��
					 {
							Slope_450nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_450nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h != 5)
							{
								TestValue_450nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 								y = y - 15;
								y = y - 23;
					 }																					//׼����0.5A,1.0A����
				}
				else if(Wavelength_Main == 3)                               //492nm����
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
						Slope_492nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
						Intercept_492nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
						if(h != 5)
						{
							TestValue_492nm[j+4][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
						}
					}
					else if(Number_Channel == 0x00)														//ABCDͨ��
					{
						Slope_492nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
						Intercept_492nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
						if(h != 5)
						{
							TestValue_492nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
						}

					 }
					 y += 4;
					 if(h == 5)
					 {
// 								y = y - 15;
								y = y - 23;
					 }																					//׼����0.5A,1.0A����
				} 
				else if(Wavelength_Main == 4)              //630nm
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
						Slope_630nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
						Intercept_630nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
						if(h != 5)
						{
							TestValue_630nm[j+4][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCEͨ��
					 {
							Slope_630nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_630nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h != 5)
							{
								TestValue_630nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//׼����0.5A,1.0A����
				}
				else if(Wavelength_Main == 5)              //��չ�˹�Ƭ01
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
						Slope_Ext01nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
						Intercept_Ext01nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
						if(h != 5)
						{
							TestValue_Ext01nm[j+4][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCEͨ��
					 {
							Slope_Ext01nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_Ext01nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h != 5)
							{
								TestValue_Ext01nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//׼����0.5A,1.0A����
				}
				else if(Wavelength_Main == 6)              //��չ�˹�Ƭ02
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
						Slope_Ext02nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
						Intercept_Ext02nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
						if(h != 5)
						{
							TestValue_Ext02nm[j+4][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCEͨ��
					 {
							Slope_Ext02nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_Ext02nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h != 5)
							{
								TestValue_Ext02nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//׼����0.5A,1.0A����
				}
				else if(Wavelength_Main == 7)              //��չ�˹�Ƭ03
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
						Slope_Ext03nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
						Intercept_Ext03nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
						if(h != 5)
						{
							TestValue_Ext03nm[j+4][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCEͨ��
					 {
							Slope_Ext03nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_Ext03nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h != 5)
							{
								TestValue_Ext03nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//׼����0.5A,1.0A����
				}
				else if(Wavelength_Main == 8)              //��չ�˹�Ƭ04
				{
					if(Number_Channel == 0x01)									//EFGHͨ��
					{   
						Slope_Ext04nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
						Intercept_Ext04nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
						if(h != 5)
						{
							TestValue_Ext04nm[j+4][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCEͨ��
					 {
							Slope_Ext04nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//�����ͨ����Kֵ(б��)
							Intercept_Ext04nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//�����ͨ����bֵ���ؾࣩ
							if(h != 5)
							{
								TestValue_Ext04nm[j][h] = ABS_Aver_Array[y];								//��0.2A,0.5A,1.0A����ֵ��������TestValue_405nm[][]��
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//׼����0.5A,1.0A����
				}
				k -= 4;															//ȡ����һ�ε�ֵ
				i --;
				h ++;															//ȡ����һ�ε�ֵ
				a1 = 0;															//��0
				a2 = 0;
				c1 = 0;
				c2 = 0;
			}												 
		}
		m = 0;						 			
		i = 0;
		h = 0;
		j ++;
		k = j;
	}
	
	j = 0;
	m = 0;
	n = 0;
	y = 0;
	
	
	
  //����������K ,B
  //��ʱ��������������K,B���������Եõ���ABS	
	m = 0;
	n = 0;
	if(Wavelength_Main == 1)       //405nm����
	{
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][1] + Intercept_405nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][1] + Intercept_405nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][1])										  //С���趨����ֵ
			   		 {
					  	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][1] + Intercept_405nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					   }	
					   else																			                   //�����趨����ֵ
					   {
					  	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][2] + Intercept_405nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][1])										  //С���趨����ֵ
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][1] + Intercept_405nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					      }	
					      else																	                   //�����趨����ֵ
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][2] + Intercept_405nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][2])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][2] + Intercept_405nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][3] + Intercept_405nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][2])										 //С���趨����ֵ
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][2] + Intercept_405nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][3] + Intercept_405nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//����1.5A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][3])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][3] + Intercept_405nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][4] + Intercept_405nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][3])										 //С���趨����ֵ
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][3] + Intercept_405nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][4] + Intercept_405nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//����2.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][4])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][4] + Intercept_405nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][5] + Intercept_405nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][4])										 //С���趨����ֵ
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][4] + Intercept_405nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][5] + Intercept_405nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else																					//����3.0A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][5] + Intercept_405nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][5] + Intercept_405nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	     }	         
	   }
		 m = 0;
	}
	else if(Wavelength_Main == 2)			                 //450nm����
	{ 
		  for(Num_Test=0;Num_Test<10;Num_Test++)
	    {
		    for(i=0;i<24;i++)
		    {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][1] + Intercept_450nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][1] + Intercept_450nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][1] + Intercept_450nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																			                   //�����趨����ֵ
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][2] + Intercept_450nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][1] + Intercept_450nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																	                   //�����趨����ֵ
					    {
					  	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][2] + Intercept_450nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)						//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][2])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][2] + Intercept_450nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][3] + Intercept_450nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  else															 //ABCFͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][2])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][2] + Intercept_450nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][3] + Intercept_450nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)						//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][3])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][3] + Intercept_450nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][4] + Intercept_450nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  else															 //ABCFͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][3])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][3] + Intercept_450nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][4] + Intercept_450nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)						//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][4])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][4] + Intercept_450nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][5] + Intercept_450nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  else															 //ABCFͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][4])										 //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][4] + Intercept_450nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					     }	
					     else																				 //�����趨����ֵ
					     {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][5] + Intercept_450nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else																					//����1.5A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][5] + Intercept_450nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][5] + Intercept_450nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	     }	         
	   }
		 m = 0;
	 }
   else if(Wavelength_Main == 3)		                 //492nm���� 
   { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][1] + Intercept_492nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][1] + Intercept_492nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][1] + Intercept_492nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																			                   //�����趨����ֵ
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][2] + Intercept_492nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][1])										  //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][1] + Intercept_492nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					      }	
					      else																	                   //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][2] + Intercept_492nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					      }
					  }

					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	  m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][2])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][2] + Intercept_492nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][3] + Intercept_492nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][2])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][2] + Intercept_492nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][3] + Intercept_492nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					   m = 0;
						 n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][3])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][3] + Intercept_492nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][4] + Intercept_492nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][3])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][3] + Intercept_492nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][4] + Intercept_492nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					   m = 0;
						 n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][4])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][4] + Intercept_492nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][5] + Intercept_492nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][4])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][4] + Intercept_492nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][5] + Intercept_492nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					   m = 0;
						 n = 0;
					  }
				 }
				 else																					//����1.5A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][5] + Intercept_492nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][5] + Intercept_492nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	     }	         
	   }
		 m = 0;
	 }
	 else if(Wavelength_Main == 4)				//630nm����
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][1] + Intercept_630nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][1] + Intercept_630nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][1] + Intercept_630nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																			                   //�����趨����ֵ
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][2] + Intercept_630nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][1])										  //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][1] + Intercept_630nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					      }	
					      else																	                   //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][2] + Intercept_630nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][2])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][2] + Intercept_630nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][3] + Intercept_630nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][2])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][2] + Intercept_630nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][3] + Intercept_630nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][3])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][3] + Intercept_630nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][4] + Intercept_630nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][3])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][3] + Intercept_630nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][4] + Intercept_630nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][4])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][4] + Intercept_630nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][5] + Intercept_630nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][4])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][4] + Intercept_630nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][5] + Intercept_630nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//����1.5A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][5] + Intercept_630nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][5] + Intercept_630nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 5)				//��չ����01
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][1] + Intercept_Ext01nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][1] + Intercept_Ext01nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][1] + Intercept_Ext01nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																			                   //�����趨����ֵ
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][2] + Intercept_Ext01nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][1])										  //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][1] + Intercept_Ext01nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					      }	
					      else																	                   //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][2] + Intercept_Ext01nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][2])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][2] + Intercept_Ext01nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][3] + Intercept_Ext01nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][2])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][2] + Intercept_Ext01nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][3] + Intercept_Ext01nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][3])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][3] + Intercept_Ext01nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][4] + Intercept_Ext01nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][3])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][3] + Intercept_Ext01nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][4] + Intercept_Ext01nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][4])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][4] + Intercept_Ext01nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][5] + Intercept_Ext01nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][4])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][4] + Intercept_Ext01nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][5] + Intercept_Ext01nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//����1.5A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][5] + Intercept_Ext01nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][5] + Intercept_Ext01nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 6)				//��չ����02
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][1] + Intercept_Ext02nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][1] + Intercept_Ext02nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][1] + Intercept_Ext02nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																			                   //�����趨����ֵ
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][2] + Intercept_Ext02nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][1])										  //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][1] + Intercept_Ext02nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					      }	
					      else																	                   //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][2] + Intercept_Ext02nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][2])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][2] + Intercept_Ext02nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][3] + Intercept_Ext02nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][2])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][2] + Intercept_Ext02nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][3] + Intercept_Ext02nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][3])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][3] + Intercept_Ext02nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][4] + Intercept_Ext02nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][3])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][3] + Intercept_Ext02nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][4] + Intercept_Ext02nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][4])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][4] + Intercept_Ext02nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][5] + Intercept_Ext02nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][4])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][4] + Intercept_Ext02nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][5] + Intercept_Ext02nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//����1.5A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][5] + Intercept_Ext02nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][5] + Intercept_Ext02nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 7)				//��չ����03
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][1] + Intercept_Ext03nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][1] + Intercept_Ext03nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][1] + Intercept_Ext03nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																			                   //�����趨����ֵ
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][2] + Intercept_Ext03nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][1])										  //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][1] + Intercept_Ext03nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					      }	
					      else																	                   //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][2] + Intercept_Ext03nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][2])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][2] + Intercept_Ext03nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][3] + Intercept_Ext03nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][2])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][2] + Intercept_Ext03nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][3] + Intercept_Ext03nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][3])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][3] + Intercept_Ext03nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][4] + Intercept_Ext03nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][3])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][3] + Intercept_Ext03nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][4] + Intercept_Ext03nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][4])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][4] + Intercept_Ext03nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][5] + Intercept_Ext03nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][4])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][4] + Intercept_Ext03nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][5] + Intercept_Ext03nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//����1.5A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][5] + Intercept_Ext03nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][5] + Intercept_Ext03nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 8)				//��չ����04
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGHͨ��
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][1] + Intercept_Ext04nm[m+4][1];	//����0.2A��׼����ABS��ֵ
		       }
					 else														//ABCDͨ��
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][1] + Intercept_Ext04nm[m][1];	//����0.2A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //����0.5A��׼����ABS��ֵ
		     {  
					 if(Number_Channel == 0x01)									//EFGHͨ��
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][1])										  //С���趨����ֵ
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][1] + Intercept_Ext04nm[m+4][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					    }	
					    else																			                   //�����趨����ֵ
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][2] + Intercept_Ext04nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					    }
					  }
					  else														   	//ABCDͨ��
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][1])										  //С���趨����ֵ
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][1] + Intercept_Ext04nm[m][1];			 //ѡ��0.2-0.5A�ε�K��Bֵ						  
					      }	
					      else																	                   //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][2] + Intercept_Ext04nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][2])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][2] + Intercept_Ext04nm[m+4][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][3] + Intercept_Ext04nm[m+4][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][2])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][2] + Intercept_Ext04nm[m][2];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][3] + Intercept_Ext04nm[m][3];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][3])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][3] + Intercept_Ext04nm[m+4][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][4] + Intercept_Ext04nm[m+4][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][3])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][3] + Intercept_Ext04nm[m][3];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][4] + Intercept_Ext04nm[m][4];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//����1.0A��׼����ABS��ֵ
				 {
					  if(Number_Channel == 0x01)									//EFGHͨ��
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][4])										 //С���趨����ֵ
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][4] + Intercept_Ext04nm[m+4][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][5] + Intercept_Ext04nm[m+4][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  else															 //ABCFͨ��
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][4])										 //С���趨����ֵ
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][4] + Intercept_Ext04nm[m][4];			 //ѡ��0.5-1.0A�ε�K��Bֵ						  
					      }	
					      else																				 //�����趨����ֵ
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][5] + Intercept_Ext04nm[m][5];			 //ѡ��1.0-1.5A�ε�K��Bֵ
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//����1.5A��׼����ABS��ֵ
				 {
				 	 if(Number_Channel == 0x01)									//EFGHͨ��
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][5] + Intercept_Ext04nm[m+4][5];				 //����1.5A��׼����ABS��ֵ
		       }
					 else														 //ABCDͨ��
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][5] + Intercept_Ext04nm[m][5];				 //����1.5A��׼����ABS��ֵ
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 
  //��10���߰��ABS����ֵƽ��ֵ
	result = 0;														 
	for(n=0;n<24;n++)													
	{
		for(Num_Test=0;Num_Test<10;Num_Test++)
		{
			result += ABS_Array2[Num_Test][ n ];						 //��ABS��ʾֵ���ۼӺ�
		}
		ABS_Aver_Array2[n] = result / 10;									 //����ABS����ֵ��ƽ��ֵ
		result = 0;														 //���ۼӱ���
	}
	
	
	CV_SD_Function(ABS_Array2,ABS_Aver_Array2,10);						 //���ú���������CVֵ��SDֵ
	 
	//����������µ�׼ȷ��
	if(Wavelength_Main == 1)//||(Wavelength_Main == 5))	20110421		                     //405nm���� or 412
	{ 	
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_405nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
		n = 0;
	}
	else if(Wavelength_Main == 2)			                 //450nm����
	{
	    n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_450nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
	    n = 0;
	}
	else if(Wavelength_Main == 3)//||(Wavelength_Main == 6))	20110421		                 //492nm����
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_492nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 4)						//630nm����
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_630nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 5)						//��չ����01
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext01nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 6)						//��չ����02
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext02nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 7)						//��չ����03
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext03nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 8)						//��չ����04
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext04nm[n]);			 //��׼ȷ��
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //ѡ���׼��İ�ֵ
			{
			    n ++;
			}
		}
		n = 0;
	}
	 
	m = 0;
	result = 0;									 
	for(n=0;n<24;n++)									
  {																			  //��ͨ����ֵ
		 result += ABS_Aver_Array2[n] ;
		 if(n == 3 || n == 7 || n == 11 || n == 15 || n == 19 || n == 23)
		 {
			 result    /= 4;
			 Differ[m] = result;
			 result    = 0;
			 m ++;
		 }
	}

	CV_SD_Function1(ABS_Aver_Array2,Differ,6);									//��ͨ����
	
	//���� K,B,T
}


//****4����׼����***��ABCDͨ����EFGHͨ��Ҫ���͵�16��ABSֵ��16��׼ȷ�ȡ�16��CVֵ��4���׼��ֵ������ABS_Send[]������*******//

//****6����׼����***��ABCDͨ����EFGHͨ��Ҫ���͵�24��ABSֵ��24��׼ȷ�ȡ�24��CVֵ��6���׼��ֵ������ABS_Send[]������*******//
void Array_Combination_Optical(u8 Wavelength_Main,u8 Number_Channel)	  
{	
  u8 i,j,m,n;
	m = 0;
	n = 0;
	j = 0;
	if(Wavelength_Main == 1)			 //405nm���� 
	{			 
		if(Number_Channel ==1)			 //EFGHͨ��
	  {
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
		      }
		      else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��6��Kֵ��
		      {
			       ABS_Send[i] = Slope_405nm[m+4][n];			 //��ȡKֵ					
				     n ++;
				     if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����24��bֵ��ÿ��ͨ��4��bֵ��
		      {
			      ABS_Send[i] = Intercept_405nm[m+4][n];			 //��ȡbֵ					
				    n ++;
				    if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_405nm[m+4][n];			 //��ȡTֵ					
				     n ++;
				     if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCDͨ��
		{
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
				 }
				 else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��4��Kֵ��
				 {
			      ABS_Send[i] = Slope_405nm[m][n];			 //��ȡKֵ					
				    n ++;
				    if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����16��bֵ��ÿ��ͨ��4��bֵ��
				 {
						ABS_Send[i] = Intercept_405nm[m][n];			 //��ȡbֵ					
						n ++;
						if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_405nm[m][n];			 //��ȡTֵ					
					 n ++;
					 if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}
	}
  else if(Wavelength_Main == 2)							 //450nm����
	{			 
		if(Number_Channel ==1)			 //EFGHͨ��
	  {
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
		      }
		      else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��6��Kֵ��
		      {
			       ABS_Send[i] = Slope_450nm[m+4][n];			 //��ȡKֵ					
				     n ++;
				     if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����24��bֵ��ÿ��ͨ��4��bֵ��
		      {
			      ABS_Send[i] = Intercept_450nm[m+4][n];			 //��ȡbֵ					
				    n ++;
				    if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_450nm[m+4][n];			 //��ȡTֵ					
				     n ++;
				     if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCDͨ��
		{
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
				 }
				 else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��4��Kֵ��
				 {
			      ABS_Send[i] = Slope_450nm[m][n];			 //��ȡKֵ					
				    n ++;
				    if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����16��bֵ��ÿ��ͨ��4��bֵ��
				 {
						ABS_Send[i] = Intercept_450nm[m][n];			 //��ȡbֵ					
						n ++;
						if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_450nm[m][n];			 //��ȡTֵ					
					 n ++;
					 if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}
	}
	else if(Wavelength_Main == 3)                           //492nm����
	{			 
		if(Number_Channel ==1)			 //EFGHͨ��
	  {
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
		      }
		      else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��6��Kֵ��
		      {
			       ABS_Send[i] = Slope_492nm[m+4][n];			 //��ȡKֵ					
				     n ++;
				     if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����24��bֵ��ÿ��ͨ��4��bֵ��
		      {
			      ABS_Send[i] = Intercept_492nm[m+4][n];			 //��ȡbֵ					
				    n ++;
				    if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_492nm[m+4][n];			 //��ȡTֵ					
				     n ++;
				     if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCDͨ��
		{
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
				 }
				 else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��4��Kֵ��
				 {
			      ABS_Send[i] = Slope_492nm[m][n];			 //��ȡKֵ					
				    n ++;
				    if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����16��bֵ��ÿ��ͨ��4��bֵ��
				 {
						ABS_Send[i] = Intercept_492nm[m][n];			 //��ȡbֵ					
						n ++;
						if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_492nm[m][n];			 //��ȡTֵ					
					 n ++;
					 if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}
	}
	else if(Wavelength_Main == 4)			                 //630nm����
	{			 
		if(Number_Channel ==1)			 //EFGHͨ��
	  {
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
		      }
		      else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��6��Kֵ��
		      {
			       ABS_Send[i] = Slope_630nm[m+4][n];			 //��ȡKֵ					
				     n ++;
				     if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����24��bֵ��ÿ��ͨ��4��bֵ��
		      {
			      ABS_Send[i] = Intercept_630nm[m+4][n];			 //��ȡbֵ					
				    n ++;
				    if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_630nm[m+4][n];			 //��ȡTֵ					
				     n ++;
				     if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCDͨ��
		{
			 for(i=0;i<146;i++)										     //ѭ�����Ʒ���96������
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //�����׼����24��ABS��ʾ��ֵ
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //�����׼����24��׼ȷ��ֵ   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //�����׼����24��CVֵ
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //�����׼����6��ͨ����ֵ
				 }
				 else if(i>=78 && i<102)									 //����4��ͨ����ABCD��EFGH����24��Kֵ��ÿ��ͨ��4��Kֵ��
				 {
			      ABS_Send[i] = Slope_630nm[m][n];			 //��ȡKֵ					
				    n ++;
				    if(n == 6)									 //һ��ͨ����4��Kֵ��ȡ���
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4ͨ��Kֵ��ȡ���M��0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //����4��ͨ����ABCD��EFGH����16��bֵ��ÿ��ͨ��4��bֵ��
				 {
						ABS_Send[i] = Intercept_630nm[m][n];			 //��ȡbֵ					
						n ++;
						if(n == 6)									         //һ��ͨ����4��bֵ��ȡ���
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4ͨ��bֵ��ȡ���M��0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_630nm[m][n];			 //��ȡTֵ					
					 n ++;
					 if(n == 5)									 //һ��ͨ����4��Kֵ��ȡ���
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4ͨ��Kֵ��ȡ���M��0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}	
	}
}

void USB_Out_K_B_Abs(u8 mode,u8 wave)
{
	u8 i=0;
	u8 j=0;
	float *addr_array_k;
	float *addr_array_b;
	float *addr_array_t;
	float *std;
	switch(wave)
	{
		case 1:
			addr_array_k = (float*)&Slope_405nm[0][0];
			addr_array_b = (float*)&Intercept_405nm[0][0];
			addr_array_t = (float*)&TestValue_405nm[0][0];
		  std          = (float*)&StandardValue_405nm[0];
			break;
		case 2:
			addr_array_k = (float*)&Slope_450nm[0][0];
			addr_array_b = (float*)&Intercept_450nm[0][0];
			addr_array_t = (float*)&TestValue_450nm[0][0];
		  std          = (float*)&StandardValue_450nm[0];
			break;
		case 3:
			addr_array_k = (float*)&Slope_492nm[0][0];
			addr_array_b = (float*)&Intercept_492nm[0][0];
			addr_array_t = (float*)&TestValue_492nm[0][0];
		  std          = (float*)&StandardValue_492nm[0];
			break;
		case 4:
			addr_array_k = (float*)&Slope_630nm[0][0];
			addr_array_b = (float*)&Intercept_630nm[0][0];
			addr_array_t = (float*)&TestValue_630nm[0][0];
		  std          = (float*)&StandardValue_630nm[0];
			break;
		default:
			break;
	}
	
	if(mode == 1)//���ԭʼADֵ
	{
		for(i=0;i<10;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("AD_Alllight[%d][%d]=%.4f\r\n",i,j,AD_Alllight[i][j]);
			}
		}
		
		for(i=0;i<10;i++)
		{
			for(j=0;j<24;j++)
			{
				printf("AD_Halflight[%d][%d]=%.4f\r\n",i,j,AD_Halflight[i][j]);
			}
		}
	}
	else if(mode == 2)//���ԭʼABS
	{
		for(i=0;i<10;i++)
		{
			for(j=0;j<24;j++)
			{
				printf("ABS_Array[%d][%d]=%0.4f\r\n",i,j,ABS_Array[i][j]);
			}
		}
		
		for(i=0;i<24;i++)
		{
			printf("ABS_Aver_Array[%d]=%0.4f\r\n",i,ABS_Aver_Array[i]);
		}
	}
	else if(mode == 3)//����������ABS
	{
		
		for(i=0;i<10;i++)
		{
			for(j=0;j<24;j++)
			{
				printf("ABS_Array2[%d][%d]=%0.4f\r\n",i,j,ABS_Array2[i][j]);
			}
		}
		
		for(i=0;i<24;i++)
		{
			printf("ABS_Aver_Array2[%d]=%0.4f\r\n",i,ABS_Aver_Array2[i]);
		}
		
		
		//���  Slope_450nm K,B,T
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
				printf("Slope_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_k);
				addr_array_k = addr_array_k+1;
			}
		}
		
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
				printf("Intercept_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_b);
				addr_array_b = addr_array_b+1;
			}
		}

		for(i=0;i<8;i++)
		{
			for(j=0;j<5;j++)
			{
				printf("TestValue_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_t);
				addr_array_t = addr_array_t+1;
			}
		}
		
	}
	else if(mode == 4)//�����Ч����--������У��
	{
		for(i=0;i<146;i++)
		{
				printf(":%0.4f",ABS_Send[i]);
		}
		
		for(i=1;i<7;i++)
		{
				printf(":%0.4f",std[i]);
		}
	}
 }


/****************************************************************************
* ��    �ƣ�Data_Handling_LiquidSamples(u8 Wavelength_Main)
* ��    �ܣ����˫������õ� AD ֵ �� ����ĵ���ABS
* ��ڲ�����u8 Wavelength_Main :
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void Data_Handling_LiquidSamples(u8 Wavelength_Main)
{
	//uchar data k = 0;
	u8 n = 0;
	u8 m = 0;
	u8 Num_Test = 0;								//�߰���Դ���
// 	u8 Num_Glass = 0;
// 	u8 Num_PerGlass = 0;
// 	float A_BS;	
	float result = 0;							//�ۼӺͱ���		    
// 	uint  Max_AD,Meas_AD;					            //����ȱ���
// 	u8 Num_Queue = 0;
// 	Num_Channel = 8;										//��ͨ����

	//���������ABS��ʾֵ������
	Num_Test = 0;													 
	if(Wavelength_Main == 1)                    //405nm����,����m��������8��ͨ����ѡ��
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
			 for(n=0;n<16;n++)
			 {
				 if(ABS_Array[Num_Test][n] < TestValue_405nm[m][1])											               //��С��0.5A����ֵ
				 {
			   	   if(ABS_Array[Num_Test][n] < TestValue_405nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][0] + Intercept_405nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][1] + Intercept_405nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
				 }
				 else	if(ABS_Array[Num_Test][n] < TestValue_405nm[m][3])																								   ///��С��1.5A����ֵ
				 {
			   	   if(ABS_Array[Num_Test][n]  < TestValue_405nm[m][2])													   //��С��1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][2] + Intercept_405nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][3] + Intercept_405nm[m][3];   //����1.0-1.5A��ABSֵ
				     }
			   }
				 else																									   ///�����ڵ���1.5A����ֵ
				 {
			   	   if(ABS_Array[Num_Test][n]  < TestValue_405nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][4] + Intercept_405nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][5] + Intercept_405nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			   }
				 
				m ++;
				if(n == 7)																						            //���ǵ�1�����һ��ͨ����n=7��
				{
					m = 0;																						            //�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
		 }	         
		 m = 0;
	 }
	}
	else if(Wavelength_Main == 2)			                 //450nm����
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_450nm[m][1])											               //��С��0.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n] < TestValue_450nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][0] + Intercept_450nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][1] + Intercept_450nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
			     }
					 else if(ABS_Array[Num_Test][n] < TestValue_450nm[m][3])											               //��С��1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n] < TestValue_450nm[m][2])									                   //��С��1.0A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][2] + Intercept_450nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][3] + Intercept_450nm[m][3];	   //����1.0-1.5A��ABSֵ
				     }
			     }
			     else																									   ///�����ڵ���1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_450nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][4] + Intercept_450nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][5] + Intercept_450nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			     }
				m ++;
				if(n == 7)																						            //���ǵ�1�����һ��ͨ����n=7��
				{
				 	m = 0;																						            //�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
			 }	         
		 m = 0;
	 }
	}
	else if(Wavelength_Main == 3)     //492nm����
	{ 
		for(Num_Test=0;Num_Test<10;Num_Test++)
	  {
			 for(n=0;n<16;n++)
			 {
			     if(ABS_Array[Num_Test][n] < TestValue_492nm[m][1])											               //��С��0.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n] < TestValue_492nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][0] + Intercept_492nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][1] + Intercept_492nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
			     }
					 else	if(ABS_Array[Num_Test][n] < TestValue_492nm[m][3])								//��С��1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_492nm[m][2])													   //��С��1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][2] + Intercept_492nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][3] + Intercept_492nm[m][3];   //����1.0-1.5A ��ABSֵ
				     }
			     }
			     else																									   ///�����ڵ���1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_492nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][4] + Intercept_492nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][5] + Intercept_492nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			     }
				m ++;
				if(n == 7)																						            //���ǵ�1�����һ��ͨ����n=7��
				{
				 	m = 0;																						            //�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
		 }	         
		 m = 0;
	 }
	}
	else if(Wavelength_Main == 4)		     //630nm����
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_630nm[m][1])											               //��С��0.5A����ֵ
			     {
			   	     if(ABS_Array[Num_Test][n] < TestValue_630nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][0] + Intercept_630nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][1] + Intercept_630nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_630nm[m][3])				   //��С��  1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_630nm[m][2])													   //��С��1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][2] + Intercept_630nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][3] + Intercept_630nm[m][3];   //����1.0-1.5A�����1.5A��ABSֵ
				     }
			     }
			     else																									   ///�����ڵ���1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_630nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][4] + Intercept_630nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][5] + Intercept_630nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			     }
				m ++;
				if(n == 7)																						            //���ǵ�1�����һ��ͨ����n=7��
				{
				 	m = 0;																						            //�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
			 }	         
		 m = 0;
	 }
	}														 
	else if(Wavelength_Main == 5)		     //��չ����01
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext01nm[m][1])											               //��С��0.5A����ֵ
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext01nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][0] + Intercept_Ext01nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][1] + Intercept_Ext01nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext01nm[m][3])				   //��С��  1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext01nm[m][2])													   //��С��1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][2] + Intercept_Ext01nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][3] + Intercept_Ext01nm[m][3];   //����1.0-1.5A�����1.5A��ABSֵ
				     }
			     }
			     else																									   ///�����ڵ���1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext01nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][4] + Intercept_Ext01nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][5] + Intercept_Ext01nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			     }
				m ++;
				if(n == 7)																						            //���ǵ�1�����һ��ͨ����n=7��
				{
				 	m = 0;																						            //�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
			 }	         
		 m = 0;
	 }
	}						    
	else if(Wavelength_Main == 6)		     //��չ����02
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext02nm[m][1])											               //��С��0.5A����ֵ
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext02nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][0] + Intercept_Ext02nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][1] + Intercept_Ext02nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext02nm[m][3])				   //��С��  1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext02nm[m][2])													   //��С��1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][2] + Intercept_Ext02nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][3] + Intercept_Ext02nm[m][3];   //����1.0-1.5A�����1.5A��ABSֵ
				     }
			     }
			     else																									   ///�����ڵ���1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext02nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][4] + Intercept_Ext02nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][5] + Intercept_Ext02nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			     }
				m ++;
				if(n == 7)											                        //���ǵ�1�����һ��ͨ����n=7��
				{
				 	m = 0;																	//�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
			 }	         
		 m = 0;
	 }
	}//XX			
	else if(Wavelength_Main == 7)		     //��չ����03
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext03nm[m][1])											               //��С��0.5A����ֵ
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext03nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][0] + Intercept_Ext03nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][1] + Intercept_Ext03nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext03nm[m][3])				   //��С��  1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext03nm[m][2])													   //��С��1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][2] + Intercept_Ext03nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][3] + Intercept_Ext03nm[m][3];   //����1.0-1.5A�����1.5A��ABSֵ
				     }
			     }
			     else																									   ///�����ڵ���1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext03nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][4] + Intercept_Ext03nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][5] + Intercept_Ext03nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			     }
				m ++;
				if(n == 7)											                        //���ǵ�1�����һ��ͨ����n=7��
				{
				 	m = 0;																	//�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
			 }	         
		 m = 0;
	 }
	}//XX			
	else if(Wavelength_Main == 8)		     //��չ����04
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext04nm[m][1])											               //��С��0.5A����ֵ
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext04nm[m][0])									                   //��С��0.2A����ֵ
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][0] + Intercept_Ext04nm[m][0];	   //����0-0.2A��ABSֵ
				     }
				     else																					               //�����ڵ���0.2С��0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][1] + Intercept_Ext04nm[m][1];	   //����0.2-0.5A��ABSֵ
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext04nm[m][3])				   //��С��  1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext04nm[m][2])													   //��С��1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][2] + Intercept_Ext04nm[m][2];	   //����0.5-1.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][3] + Intercept_Ext04nm[m][3];   //����1.0-1.5A�����1.5A��ABSֵ
				     }
			     }
			     else																									   ///�����ڵ���1.5A����ֵ
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext04nm[m][4])													   //��С��2.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][4] + Intercept_Ext04nm[m][4];	   //����1.5-2.0A��ABSֵ
				     }
				     else																								   //�����ڵ���1.0A����ֵ
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][5] + Intercept_Ext04nm[m][5];   //����2.0-3.0A�����3.0A��ABSֵ
				     }
			     }
				m ++;
				if(n == 7)											                        //���ǵ�1�����һ��ͨ����n=7��
				{
				 	m = 0;																	//�����ǵ�һ�����һ��ͨ��ʱm����0ʹk��bֵ�ٴӵ�1ͨ��ȡ��
				}
			 }	         
		 m = 0;
	 }
	}//XX			
	
	for(n=0;n<16;n++)												 //��10���߰���ǰ8���߰��ABS��ʾƽ��ֵ
	{
		for(Num_Test=0;Num_Test<10;Num_Test++)
		{
			result += ABS_Array2[Num_Test][ n ];					     //��ÿ����ǰ10���߰��ABS��ʾֵ�ۼӺ�
		}
		ABS_Aver_Array2[n] = result / 10;								//��ÿ����ǰ10���߰��ABS��ʾֵƽ��ֵ
		result = 0;													//�ۼӺͱ�����0׼������һ��ǰ10���߰��ABS��ʾֵ�ۼӺ�
	}																	

	CV_SD_Function(ABS_Array2,ABS_Aver_Array2,10);						//���ú�����Һ������CVֵ��SDֵ
	
}


/****************************************************************************
* ��    �ƣ�USB_Out_Sample_Abs(u8 wave_flag,u8 out_mode)
* ��    �ܣ����˫������õ� AD ֵ �� ����ĵ���ABS
* ��ڲ�����u8 wave_flag :0 :��������  1��˫����
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void USB_Out_Sample_Abs(u8 wave_flag,u8 out_mode)
{
	u8 i=0;
	u8 j=0;
	
	if (out_mode == 0)
	{
		//������ ADֵ ����������
		for(i=0;i<8;i++)
		{
			printf("AD_ALLlight_Single[%d]=%.4f\r\n",i,AD_ALLlight_Single[i]);
		}
		
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("AD_Halflight_Single[%d][%d]=%.4f\r\n",i,j,AD_Halflight_Single[i][j]);
			}
		}
		
		//����������� ԭʼABS
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("ABS_SingleWave0[%d][%d]=%0.4f\r\n",i,j,ABS_SingleWave0[i][j]);
			}
		}
		
		//����������� �������ABS	
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("ABS_SingleWave[%d][%d]=%0.4f\r\n",i,j,ABS_SingleWave[i][j]);
			}
		}
		
		if(wave_flag == 1)//˫���� AD ֵ (�β���)
		{
			for(i=0;i<8;i++)
			{
				printf("AD_ALLlight_Double[%d]=%.4f\r\n",i,AD_ALLlight_Double[i]);
			}
			
			for(i=0;i<12;i++)
			{
				for(j=0;j<8;j++)
				{
					printf("AD_Halflight_Double[%d][%d]=%.4f\r\n",i,j,AD_Halflight_Double[i][j]);
				}
			}
			
		//����β����� ԭʼABS
			for(i=0;i<12;i++)
			{
				for(j=0;j<8;j++)
				{
					printf("ABS_DoubleWave0[%d][%d]=%0.4f\r\n",i,j,ABS_DoubleWave0[i][j]);
				}
			}
			
		//����β����� �������ABS	
			for(i=0;i<12;i++)
			{
				for(j=0;j<8;j++)
				{
					printf("ABS_DoubleWave[%d][%d]=%0.4f\r\n",i,j,ABS_DoubleWave[i][j]);
				}
			}
		}
	}
	else if(out_mode == 1)
	{
		//����������� �������ABS	
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf(":%0.4f",ABS_SingleWave[i][j]);
			}
		}
	}
}

/****************************************************************************
* ��    �ƣ�u8 AD_Loop_Sample(u16* addr)
* ��    �ܣ�
* ��ڲ�����u16* addr : AD�ɼ�ֵ��ŵĵ�ַ
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
u8 AD_Loop_Sample(float* addr)
{
 	u8 Response_CMD[3]={0x68,0x00,0xff};
	float *address = 0;
	u8 ret =0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint16_t pluse_Max = 600;
	uint8_t count = 100;

	address = addr;
	pluse_Max = 600;
	count     = 100;
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//ʹ�ܵ��
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);//�������
	
	//�ö�λƬ�������
	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//���ס��1�� ͨ��Ϊ0
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_2);
		delay_ms(1);
		GPIO_SetBits(GPIOB,GPIO_Pin_2);
		delay_ms(1);
		pluse_Max = pluse_Max - 1;
		count = count - 1;
	}
	
	if(count==0)//ͬ������λ����������
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		Response_CMD[1]=count;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 1;
	}	
		
	//��ʽ��ʼ��λ12��
	for(i=0;i<12;i++)
	{
		//����7������ʽΪ���õ�Ƭ�ƶ�һ����룬�ö�λ��ƫ������������һ����λ��
		for(j=0;j<7;j++)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			pluse_Max = pluse_Max - 1;
		}
		
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//���ס��1�� ͨ��Ϊ0
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			pluse_Max = pluse_Max - 1;
		}
		
		if(pluse_Max == 0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			Response_CMD[1]=pluse_Max;
			USB_TxWrite(Response_CMD, 3);//������Ӧ
			return 2;//��λʧ��
		}

		delay_ms(200);
		ADC0_Sample_Mode(address,2);//�ɼ�8ͨ��
		address = address +8;
	}
	
	ret = 0;
	//ø��帴λ
	ret = Plate_Motor_Reset(1);
	if(ret)//��λʧ��
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 3;
	}
	delay_ms(10);

	return 0;
}


/****************************************************************************
* ��    �ƣ�u8 Data_Handling(u8 Wavelength_Main,u8 Wavelength_Second)
* ��    �ܣ����ݴ����������㵥/˫���������������ABSʵ��ֵ������ABS��ʾֵ��
* ��ڲ�����u16* addr : AD�ɼ�ֵ��ŵĵ�ַ
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
void Data_Handling(u8 Wavelength_Main,u8 Wavelength_Second)
{
  u8 i = 2;
	u8 k = 1;
	u8 m,n;
	float A_BS;										    //����ȱ���
	u8 Wave_Flag =0;   
	u8 Num_Queue =0;   //����
	u8 Num_Channel =8; //ÿ�е�ͨ����
// 	u16  Max_AD,Meas_AD;					  //����ȱ���
// 	u16  *Adress_result96;					//��ַ����
// 	u16  *Adress_Fulllight;			    //ȫ��Դʱ��ADֵ��ַָ��
	float  Max_AD,Meas_AD;					  //����ȱ���
	float  *Adress_result96;					//��ַ����
	float  *Adress_Fulllight;			    //ȫ��Դʱ��ADֵ��ַָ��
	float *Adress_ABS;							//ABSֵ��ַ����

	Adress_result96  = &AD_Halflight_Single[0][0];//������ 12�� 8 ͨ��AD
	Adress_Fulllight = AD_ALLlight_Single;  //������ֱ��ͨ��AD
	Adress_ABS       = &ABS_SingleWave[0][0];					    //ABSֵ��ַ����
	
	if(Wavelength_Second)
	{
		Wave_Flag = 1;//˫����
	}
	else
	{
		Wave_Flag = 0;//������
	}
	
	while(i !=0)
	{
		 while( Num_Queue != 12 )	                    //12������δ�������
		 {
			  while(Num_Channel != 0)
			  {
			      Max_AD   = *Adress_Fulllight;										//ȡ��Դֱ���µ�ADֵ
			      Meas_AD  = *Adress_result96;										  //ȡ�м����ʱ��ADֵ
			      A_BS     = (float)Max_AD / Meas_AD;                                 //����ǿ��תΪfloat�ͣ����򽫵��¼����������ȷ
			      A_BS     = log10(A_BS);                                            //���������
			      *Adress_ABS = A_BS;					                            //����ABSֵ
			      Adress_Fulllight ++;
			      Adress_result96  ++;
			      Adress_ABS ++;
			      Num_Channel --;
		 	  }

			  Num_Channel = 8;
			  Num_Queue ++ ;		                   //�ɼ���1�����б����Լ�1
			  if(k == 1)
			  {   
				   Adress_Fulllight = AD_ALLlight_Single;	   //���õ������¹�Դֱ��ʱ8ͨ����ַ
			  }
			  else
			  {
				   Adress_Fulllight = AD_ALLlight_Double;	   //����˫�����¹�Դֱ��ʱ8ͨ����ַ
			  }
	   }
		 Num_Queue = 0;
		 if( Wave_Flag == 1) //˫������
		 {
				i --;
				k --;
				Adress_Fulllight = AD_ALLlight_Double;				   //���ù�Դֱ��ʱ8ͨ����ַ
				Adress_result96  = &AD_Halflight_Double[0][0];				   //˫����96�����ݵ�ַ
				Adress_ABS       = &ABS_DoubleWave[0][0];				   //˫����96��ABSֵ��ַ
		 }
		 else//������
		 {
		   i = i - 2;
		 }
  }

//-------------------------��������----------
	for(i=0;i<12;i++)
	{
		for(k=0;k<8;k++)
		{
			ABS_SingleWave0[i][k] = ABS_SingleWave[i][k];
			ABS_DoubleWave0[i][k] = ABS_DoubleWave[i][k];
		}
	}
//-------------------
		

		
// 	ReadFlash(1);            //��flash�ж�ȡ�������Ķ���ϵ����K��bֵ������ֵ��
	//����������ģʽ��ABS��ʾֵ
	if(Wavelength_Main == 1)                        //||(Wavelength_Main == 5))	405nm���� 
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_405nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_405nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][0] + Intercept_405nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][1] + Intercept_405nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else if(ABS_SingleWave[m][n] < TestValue_405nm[n][3])																									   ///�����ڵ���0.5A����ֵ
			   {
			   	   if(ABS_SingleWave[m][n] < TestValue_405nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][2] + Intercept_405nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][3] + Intercept_405nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }				 
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	   if(ABS_SingleWave[m][n] < TestValue_405nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][4] + Intercept_405nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][5] + Intercept_405nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	    }
	}
	else if(Wavelength_Main == 2)			                     //450nm����
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_450nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_450nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][0] + Intercept_450nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][1] + Intercept_450nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_450nm[n][3])																								   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_450nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][2] + Intercept_450nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][3] + Intercept_450nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_450nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][4] + Intercept_450nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][5] + Intercept_450nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	    }
	}
	else if(Wavelength_Main == 3)                //||(Wavelength_Main == 6))	492nm���� 
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_492nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_492nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][0] + Intercept_492nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][1] + Intercept_492nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_492nm[n][3])																								   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_492nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][2] + Intercept_492nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][3] + Intercept_492nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_492nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][4] + Intercept_492nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][5] + Intercept_492nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	    }
	}
	else	if(Wavelength_Main == 4)		                                            //630nm����
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_630nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_630nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][0] + Intercept_630nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][1] + Intercept_630nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else if(ABS_SingleWave[m][n] < TestValue_630nm[n][3])																										   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_630nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][2] + Intercept_630nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][3] + Intercept_630nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_630nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][4] + Intercept_630nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][5] + Intercept_630nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 5)		                                            //��չ����01
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][0] + Intercept_Ext01nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][1] + Intercept_Ext01nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][3])																								   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][2] + Intercept_Ext01nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = TestValue_Ext01nm[m][n] * Slope_Ext01nm[n][3] + Intercept_Ext01nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][4] + Intercept_Ext01nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = TestValue_Ext01nm[m][n] * Slope_Ext01nm[n][5] + Intercept_Ext01nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 6)		                                            //��չ����02
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][0] + Intercept_Ext02nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][1] + Intercept_Ext02nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][3])																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][2] + Intercept_Ext02nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][3] + Intercept_Ext02nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][4] + Intercept_Ext02nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][5] + Intercept_Ext02nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 7)		                                            //��չ����03
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][0] + Intercept_Ext03nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][1] + Intercept_Ext03nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][3])																								   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][2] + Intercept_Ext03nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][3] + Intercept_Ext03nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][4] + Intercept_Ext03nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][5] + Intercept_Ext03nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 8)		                                            //��չ����04
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][1])											               //��С��0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][0])									                   //��С��0.2A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][0] + Intercept_Ext04nm[n][0];	   //����0-0.2A��ABSֵ
				   }
				   else																					               //�����ڵ���0.2С��0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][1] + Intercept_Ext04nm[n][1];	   //����0.2-0.5A��ABSֵ
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][3])																								   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][2])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][2] + Intercept_Ext04nm[n][2];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][3] + Intercept_Ext04nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
			   else																									   ///�����ڵ���0.5A����ֵ
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][4])													   //��С��1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][4] + Intercept_Ext04nm[n][4];	   //����0.5-1.0A��ABSֵ
				   }
				   else																								   //�����ڵ���1.0A����ֵ
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][5] + Intercept_Ext04nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
				   }
			   }
		   }
	  }
	}

	
	//����˫����ģʽ��ABS��ʾֵ														   
	if(Wave_Flag == 1)							 //˫�������¼���ABSֵ
	{
		 Wavelength_Main = Wavelength_Second;			 //�β���ֵ����������
// 		 ReadFlash(1);									 //��ȡ�β����Ķ���ϵ��
		 if(Wavelength_Second == 1)                        //||(Wavelength_Second == 5))	405nm�β���  
	   {   
	     for(m=0;m<12;m++)
	     {
		     for(n=0;n<8;n++)
		     {   
					 if(ABS_DoubleWave[m][n] < TestValue_405nm[n][1])											               //��С��0.5A����ֵ
			     {
							if(ABS_DoubleWave[m][n] < TestValue_405nm[n][0])									                   //��С��0.2A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][0] + Intercept_405nm[n][0];	   //����0-0.2A��ABSֵ
							}
							else																					               //�����ڵ���0.2С��0.5A
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][1] + Intercept_405nm[n][1];	   //����0.2-0.5A��ABSֵ
							}
					 }
					 else	if(ABS_DoubleWave[m][n] < TestValue_405nm[n][3])																								   ///�����ڵ���0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_405nm[n][2])													   //��С��1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][2] + Intercept_405nm[n][2];	   //����0.5-1.0A��ABSֵ
							}
							else																								   //�����ڵ���1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][3] + Intercept_405nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
							}
					 }
					 else																									   ///�����ڵ���0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_405nm[n][4])													   //��С��1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][4] + Intercept_405nm[n][4];	   //����0.5-1.0A��ABSֵ
							}
							else																								   //�����ڵ���1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][5] + Intercept_405nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
							}
					 }
					 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
		     }
	     }
	   }
	   else if(Wavelength_Second == 2)			                     //450nm����
	   {   
	     for(m=0;m<12;m++)
	     {
		     for(n=0;n<8;n++)
		     {   
					 if(ABS_DoubleWave[m][n] < TestValue_450nm[n][1])											               //��С��0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_450nm[n][0])									                   //��С��0.2A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][0] + Intercept_450nm[n][0];	   //����0-0.2A��ABSֵ
							}
							else																					               //�����ڵ���0.2С��0.5A
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][1] + Intercept_450nm[n][1];	   //����0.2-0.5A��ABSֵ
							}
					 }
					 else if(ABS_DoubleWave[m][n] < TestValue_450nm[n][3])																									   ///�����ڵ���0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_450nm[n][2])													   //��С��1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][2] + Intercept_450nm[n][2];	   //����0.5-1.0A��ABSֵ
							}
							else																								   //�����ڵ���1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][3] + Intercept_450nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
							}
					 }
					 else																									   ///�����ڵ���0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_450nm[n][4])													   //��С��1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][4] + Intercept_450nm[n][4];	   //����0.5-1.0A��ABSֵ
							}
							else																								   //�����ڵ���1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][5] + Intercept_450nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
							}
					 }
					 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
				 }
			 }
		 }
		 else if(Wavelength_Second == 3)                         //(Wavelength_Second == 6))			  492nm���� 
		 {   
			 for(m=0;m<12;m++)
			 {
				 for(n=0;n<8;n++)
				 {   
					 if(ABS_DoubleWave[m][n] < TestValue_492nm[n][1])											               //��С��0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_492nm[n][0])									                   //��С��0.2A����ֵ
							{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][0] + Intercept_492nm[n][0];	   //����0-0.2A��ABSֵ
							}
							else																					               //�����ڵ���0.2С��0.5A
							{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][1] + Intercept_492nm[n][1];	   //����0.2-0.5A��ABSֵ
							}
					 }
					 else	if(ABS_DoubleWave[m][n] < TestValue_492nm[n][3])																								   ///�����ڵ���0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_492nm[n][2])													   //��С��1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][2] + Intercept_492nm[n][2];	   //����0.5-1.0A��ABSֵ
							}
							else																								   //�����ڵ���1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][3] + Intercept_492nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
							}
					 }
					 else																									   ///�����ڵ���0.5A����ֵ
					 {
							if(ABS_DoubleWave[m][n] < TestValue_492nm[n][4])													   //��С��1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][4] + Intercept_492nm[n][4];	   //����0.5-1.0A��ABSֵ
							}
							else																								   //�����ڵ���1.0A����ֵ
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][5] + Intercept_492nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
							}
					 }
					 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
				 }
			 }
		}
		else if(Wavelength_Second == 4)		                                            //630nm����
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_630nm[n][1])											               //��С��0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_630nm[n][0])									                   //��С��0.2A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][0] + Intercept_630nm[n][0];	   //����0-0.2A��ABSֵ
						}
						else																					               //�����ڵ���0.2С��0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][1] + Intercept_630nm[n][1];	   //����0.2-0.5A��ABSֵ
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_630nm[n][3])																									   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_630nm[n][2])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][2] + Intercept_630nm[n][2];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][3] + Intercept_630nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 else																									   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_630nm[n][4])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][4] + Intercept_630nm[n][4];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][5] + Intercept_630nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 5)		                                            //��չ����1
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][1])											               //��С��0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][0])									                   //��С��0.2A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][0] + Intercept_Ext01nm[n][0];	   //����0-0.2A��ABSֵ
						}
						else																					               //�����ڵ���0.2С��0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][1] + Intercept_Ext01nm[n][1];	   //����0.2-0.5A��ABSֵ
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][3])																								   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][2])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][2] + Intercept_Ext01nm[n][2];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][3] + Intercept_Ext01nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 else																									   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][4])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][4] + Intercept_Ext01nm[n][4];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][5] + Intercept_Ext01nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 6)		                                            //��չ����02
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][1])											               //��С��0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][0])									                   //��С��0.2A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][0] + Intercept_Ext02nm[n][0];	   //����0-0.2A��ABSֵ
						}
						else																					               //�����ڵ���0.2С��0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][1] + Intercept_Ext02nm[n][1];	   //����0.2-0.5A��ABSֵ
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][3])																									   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][2])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][2] + Intercept_Ext02nm[n][2];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][3] + Intercept_Ext02nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 else																									   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][4])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][4] + Intercept_Ext02nm[n][4];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][5] + Intercept_Ext02nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 7)		                                            //��չ����03
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][1])											               //��С��0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][0])									                   //��С��0.2A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][0] + Intercept_Ext03nm[n][0];	   //����0-0.2A��ABSֵ
						}
						else																					               //�����ڵ���0.2С��0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][1] + Intercept_Ext03nm[n][1];	   //����0.2-0.5A��ABSֵ
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][3])																								   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][2])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][2] + Intercept_Ext03nm[n][2];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][3] + Intercept_Ext03nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 else																									   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][4])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][4] + Intercept_Ext03nm[n][4];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][5] + Intercept_Ext03nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 8)		                                            //��չ����04
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][1])											               //��С��0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][0])									                   //��С��0.2A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][0] + Intercept_Ext04nm[n][0];	   //����0-0.2A��ABSֵ
						}
						else																					               //�����ڵ���0.2С��0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][1] + Intercept_Ext04nm[n][1];	   //����0.2-0.5A��ABSֵ
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][3])																								   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][2])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][2] + Intercept_Ext04nm[n][2];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][3] + Intercept_Ext04nm[n][3];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 else																									   ///�����ڵ���0.5A����ֵ
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][4])													   //��С��1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][4] + Intercept_Ext04nm[n][4];	   //����0.5-1.0A��ABSֵ
						}
						else																								   //�����ڵ���1.0A����ֵ
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][5] + Intercept_Ext04nm[n][5];	   //����1.0-1.5A�����1.5A��ABSֵ
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//˫�������¼���ABS = ������ABS - �β���ABS
			 }
		 }
	  }			
	}

	Wave_Flag = 0;
}																									//��˫������־	

