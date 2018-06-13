/****************************************************************************
* �ļ����ƣ�func.c
* ˵    ���������ļ�
* ��    �ܣ�������Ҫ�������߼���������
* ���÷���1��
* ���÷���2��
* ���÷���3��
* ���÷���4��
* ���÷���5��
* ���÷���6��
* ���÷���7��
* ���÷���8��
* ���÷���9��
* ���÷���10��
* ���÷���11��
****************************************************************************/
#include "func.h"
#include "uart.h"
#include "crc32.h"
#include "motor.h"
#include "ds18b20.h"
#include "heat.h"
#include "usb_config.h"
#include "ad.h"
#include "calibrate.h"
#include "pbdata.h"
#include "flash.h"


/****************************************************************************
* ��    �ƣ�u8 Func_Front_Connect_Test(void)
* ��    �ܣ����������������ɹ�����
* ��ڲ�����void
* ���ڲ����������Լ�ɹ���־
* ˵    ����
* ���÷����������� 03 00 00 00 00 00 fd c1 b4 4c ff  ��ǰ�˰� ��Ӧ 30 ff
****************************************************************************/
u8 Func_Front_Connect_Test(void)
{
	u8 Response_CMD[3]={0x30,0x00,0xff};
	
	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Front_Self_Test(void)
* ��    �ܣ��Լ�������
* ��ڲ�����void
* ���ڲ����������Լ�ɹ���־
* ˵    ����
* ���÷����������� 04 
****************************************************************************/
u8 Func_Front_Self_Test(void)
{
	u8 Response_CMD[3]={0x40,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	G_Computer_Connect_Flag = 1;//��ʾ�Ѿ��ɹ������� ��λ��
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Sample_Test_Prepare(void)
* ��    �ܣ���Ʒ׼������
* ��ڲ�����void
* ���ڲ�����������Ʒ׼�����Ա�־
* ˵    ����
* ���÷����������� 06 
****************************************************************************/
u8 Func_Sample_Test_Prepare(void)
{
	u8 Response_CMD[3]={0x60,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}
/****************************************************************************
* ��    �ƣ�u8 Func_Sample_Test_Start(void)
* ��    �ܣ���ʼ��������
* ��ڲ�����void
* ���ڲ����������������Ա�־
* ˵    ����
* ���÷����������� 07
****************************************************************************/
u8 Func_Sample_Test_Start(void)
{
	u8 Response_CMD[3]={0x70,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Sample_Test_Stop(void)
* ��    �ܣ�����ֹͣ
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�08
****************************************************************************/
u8 Func_Sample_Test_Stop(void)
{
	u8 Response_CMD[3]={0x80,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Com_On(void)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�09
****************************************************************************/
u8 Func_Com_On(void)
{
	u8 Response_CMD[3]={0x90,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Com_Off(void)
* ��    �ܣ����ڹر�
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�0A
****************************************************************************/
u8 Func_Com_Off(void)
{
	u8 Response_CMD[3]={0xA0,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_USB_On(void)
* ��    �ܣ�USB����
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�0B
****************************************************************************/
u8 Func_USB_On(void)
{
	u8 Response_CMD[3]={0xB0,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_USB_Off(void)
* ��    �ܣ�USB�ر�
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�0C
****************************************************************************/
u8 Func_USB_Off(void)
{
	u8 Response_CMD[3]={0xC0,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Light_On(void)
* ��    �ܣ���Դ����
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�0D
****************************************************************************/
u8 Func_Light_On(void)
{
	u8 Response_CMD[3]={0xd0,0x00,0xff};

	GPIO_SetBits(GPIOA,GPIO_Pin_8);	//����Դ��

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Light_Off(u8 fre)
* ��    �ܣ���Դ�ر�
* ��ڲ�����u8 fre
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�0E
****************************************************************************/
u8 Func_Light_Off(u8 fre)
{
	u8 Response_CMD[3]={0xE0,0x00,0xff};

	if(fre==0)
	{
		G_Power_On_Off = 0;//�رչ�Դ����
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//�رչ�Դ��
	}
	else
	{
		G_Power_On_Off = 1;//������Դ����
		G_Power_On_Off_Fre = fre;//����ǿ��
		
	}

	USB_TxWrite(Response_CMD, 3);//������Ӧ



	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Temperature_Disp(void)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�13
****************************************************************************/
// u8 Func_Temperature_Disp(void)
// {
// 	u8 i,j;
// 	u16 temp=0;
// 	u8 Response_CMD[3]={0x31,0x00,0xff};

// 	USB_TxWrite(Response_CMD, 3);//������Ӧ

//   
//   //���ID
// 	for(i=0;i<G_Ds18b20_count;i++)
// 	{
// 		printf("18B20 ROM_ID %d = %X %X %X %X %X %X %X %X \r\n",i,G_DS18B20_ID[i][0],G_DS18B20_ID[i][1],G_DS18B20_ID[i][2],G_DS18B20_ID[i][3],G_DS18B20_ID[i][4],G_DS18B20_ID[i][5],G_DS18B20_ID[i][6],G_DS18B20_ID[i][7]);
// 		delay_ms(50);
// 	}
// 	//ǰ����ƫ��Ƚϴ�����
// 	for(i=0;i<5;i++)
// 	{
// 		for(j=0;j<G_Ds18b20_count;j++)
// 		{
// 			temp=DS18B20_Get_Temp(G_DS18B20_ID[j]);
// 			delay_ms(30);
// 		}
// 	}

// 	//����¶�
// 	for(i=0;i<20;i++)
// 	{
// 		printf("---------%d----------\r\n",i);  	  
// 		for(j=0;j<G_Ds18b20_count;j++)
// 		{
// 			temp=DS18B20_Get_Temp(G_DS18B20_ID[j]);
// 			printf("temp = %d\r\n",temp);
// 			temp = Temperature_convert(temp);
// 			if(temp&0x8000)//����
// 			{
// 				temp = temp &0x7fff;
// 				printf("current tenperature ID-%d: -%d.%d \r\n",j,temp>>8,temp&0xff);  	  
// 			}
// 			else
// 			{
// 				printf("current tenperature ID-%d: %d.%d  \r\n",j,temp>>8,temp&0xff);  	  
// 			}
// 			delay_ms(500);
// 		}
// 		
// 		if(G_Temperature_Disp ==0)
// 		{
// 			break;
// 		}
// 	}

// 	return 0;
// }

/****************************************************************************
* ��    �ƣ�u8 Func_Temperature_Hot(u16 Target)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�12 - 02
****************************************************************************/
u8 Func_Temperature_Hot(u16 Target)
{
	//uint16_t i =0;
	//uint8_t j =0;
 	//uint8_t k =0;
 	u16 cur_temp=0;
	u8 Response_CMD[3]={0x31,0x00,0xff};
	//��ʼ������Ƭ
	Heat3_Init();
	G_Heat3_Start =1;//��ʼ����
	
	//��ʼ����������
	Peltier_Init(1);
	G_Heat1_Start =1;//������ʼ����
	G_Heat2_Start = 0;//��������ر�
	//USB_TxWrite(Response_CMD, 3);//������Ӧ


	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Temperature_Cold(u16 Target)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�12 - 03
****************************************************************************/
u8 Func_Temperature_Cold(u16 Target)
{
	//uint16_t i =0;
	//uint8_t j =0;
 	//uint8_t k =0;
 	//u16 temp=0;
	u8 Response_CMD[3]={0x31,0x00,0xff};
	Peltier_Init(3);
	G_Heat2_Start =1; //������ʼ����
	G_Heat1_Start = 0;//�������ȹر�

	if(G_Heat3_Start ==1)//�������Ƭ���ڹ���״̬�����ü���Ƭ����
	{
		Heat3_Control(Target,IDLESTATE);  //����Ƭֹͣ����,��������ߵ�ƽ
		G_Heat3_Start = 0;
	}


	return 0;
}
/****************************************************************************
* ��    �ƣ�u8 Func_Temperature_Idle(u16 Target)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�12 - 04
****************************************************************************/
u8 Func_Temperature_Idle(u16 Target)
{
	u8 Response_CMD[3]={0x41,0x00,0xff};

	//USB_TxWrite(Response_CMD, 3);//������Ӧ
  Heat3_Control(Target,IDLESTATE);  //����Ƭֹͣ����,��������ߵ�ƽ
	
	if(G_Heat1_Start ==1)//��������״̬
	{
		Heat1_Control(Target,IDLESTATE);  //������ֹͣ����,��������͵�ƽ
	}
	
	if(G_Heat2_Start ==1)//��������״̬
	{
		Heat2_Control(Target,IDLESTATE);  //������ֹͣ����,��������͵�ƽ
	}
  G_Heat1_Start = 0; 
  G_Heat2_Start = 0; 
  G_Heat3_Start = 0; 
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Temperature_Heat(u16 Target)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�13
****************************************************************************/
u8 Func_Temperature_Heat(u16 Target)
{
	//uint16_t i =0;
	//uint8_t j =0;
 	//uint8_t k =0;
 	//u16 temp=0;
	u8 Response_CMD[3]={0x31,0x00,0xff};
  Heat3_Control(Target,HOTSTATE);

	//USB_TxWrite(Response_CMD, 3);//������Ӧ


	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Temperature_Off(void)
* ��    �ܣ������ر�
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�14
****************************************************************************/
u8 Func_Temperature_Off(void)
{
	u8 Response_CMD[3]={0x41,0x00,0xff};
	GPIO_InitTypeDef GPIO_InitStructure;

	TIM_Cmd(TIM4, DISABLE); 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;      //6-->heat2  8-->heat1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);               //�͵�ƽ
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);               //�͵�ƽ

	GPIO_SetBits(GPIOB,GPIO_Pin_7);                 //�ߵ�ƽ--����Ƭ
  G_Heat1_Start = 0; 
  G_Heat2_Start = 0; 
  G_Heat3_Start = 0; 
	return 0;
}



/****************************************************************************
* ��    �ƣ�u8 Func_Peltier_Board_Heat(void)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�15
****************************************************************************/
u8 Func_Peltier_Board_Heat(u16 Target)
{
	uint8_t j =0;
	u8 Response_CMD[3]={0x51,0x00,0xff};

	//USB_TxWrite(Response_CMD, 3);//������Ӧ

	Heat1_Control(Target,HOTSTATE);

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Peltier_Board_Cold(void)
* ��    �ܣ���������
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�16
****************************************************************************/
u8 Func_Peltier_Board_Cold(u16 Target)
{
	uint8_t j =0;
	u8 Response_CMD[3]={0x61,0x00,0xff};

// 	USB_TxWrite(Response_CMD, 3);//������Ӧ

  Heat2_Control(Target,COLDSTATE);
	return 0;
}



/****************************************************************************
* ��    �ƣ�u8 Func_Plate_Motor_Reset(u8 speed)
* ��    �ܣ�ͬ����������и�λ
* ��ڲ�����
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�17
****************************************************************************/
u8 Func_Plate_Motor_Reset(u8 speed)
{
	u8 Response_CMD[3]={0x71,0x00,0xff};
	u8 ret =0;

	if(speed<=0)
	{
		return 1;
	}
	
	ret = Plate_Motor_Reset(speed);
	Response_CMD[1]=ret;
	USB_TxWrite(Response_CMD, 3);//������Ӧ
	
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Plate_Motor_Run(uint16_t speed,uint16_t step)
* ��    �ܣ�ͬ�����������ָ��step
* ��ڲ�����speed ����������
* ��ڲ�����step  �����ж��ٲ�
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    �������һ��1.8�ȣ�200��һȦ �����г�600 ��step ��Ҫ����6
* ���÷������������ͣ�18
****************************************************************************/
u8 Func_Plate_Motor_Run(uint16_t speed,uint16_t step)
{
	u8 Response_CMD[3]={0x81,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

  Plate_Motor_Run(0,speed,step);//����
  Plate_Motor_Run(1,speed,step);//����
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Plate_Motor_Position(u8 index)
* ��    �ܣ�ͬ���������λ
* ��ڲ�����step �����ж��ٲ�
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    �������һ��1.8�ȣ�200��һȦ
* ���÷������������ͣ�19
****************************************************************************/
u8 Func_Plate_Motor_Position(u8 speed,u8 index)
{
	u8 Response_CMD[3]={0x91,0x00,0xff};
	u8 ret =0;
	ret = Plate_Motor_Position(speed,index);
	Response_CMD[1]=ret;

	USB_TxWrite(Response_CMD, 3);//������Ӧ
	return 0;
}


/****************************************************************************
* ��    �ƣ�u8 Func_Plate_Motor_Shake(u8 speed,u8 time)
* ��    �ܣ�ͬ���������ʱ��
* ��ڲ�����speed �����ٶ�  0 ��,1��, 2��
* ��ڲ�����time ���𶯶���ʱ�䣬 ��λ��
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�1A
****************************************************************************/
u8 Func_Plate_Motor_Shake(u8 speed,u8 time)
{
	u8 shake_speed = 2;
	u8 shake_time = 20;
	u8 Response_CMD[3]={0xA1,0x00,0xff};
	u8 ret = 0;

	
	shake_time = time;
	
	switch(speed)
	{
		case 0:
			shake_speed = 2;
		  shake_time  = time*13;
			break;
		case 1:
			shake_speed = 3;
		  shake_time  = time*10;
			break;
		case 2:
			shake_speed = 4;
		  shake_time  = time*8;
			break;
		default:
			break;
	}
	
	ret = Plate_Motor_Shake(shake_speed,shake_time);
	Response_CMD[1] = ret;
	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}


/****************************************************************************
* ��    �ƣ�u8 Func_ABS_Test(void)
* ��    �ܣ�ABS����
* ��ڲ�����
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�1B
****************************************************************************/
u8 Func_ABS_Test(void)
{
	u8 Response_CMD[3]={0xB1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_ABS_Stop(void)
* ��    �ܣ�ABS����
* ��ڲ�����
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�1C
****************************************************************************/
u8 Func_ABS_Stop(void)
{
	u8 Response_CMD[3]={0xC1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_LM_AD_Test(void)
* ��    �ܣ���ǿAD����
* ��ڲ�����
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�1D
****************************************************************************/
u8 Func_LM_AD_Test(void)
{
	u8 Response_CMD[3]={0xD1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_LM_AD_Stop(void)
* ��    �ܣ���ǿAD����ֹͣ
* ��ڲ�����
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷������������ͣ�1E
****************************************************************************/
u8 Func_LM_AD_Stop(void)
{
	u8 Response_CMD[3]={0xE1,0x00,0xff};

	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Filter_Motor_Reset(void)
* ��    �ܣ��˹��ֵ����λ
* ��ڲ�����void
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    �����������ͣ�23
* ���÷�����
****************************************************************************/
u8 Func_Filter_Motor_Reset(u8 dir,u8 speed)
{
	u8 Response_CMD[3]={0x32,0x00,0xff};
	u8 ret =0;

	ret = Filter_Motor_Reset(dir,speed);
	Response_CMD[1]=ret;
	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Filter_Motor_Position(u8 dir,u8 speed,u8 index)
* ��    �ܣ��˹��ֵ����λ
* ��ڲ�����void
* ���ڲ��������ض�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����8�ģ� �ܹ�400��һȦ,1��0.9��,  8���˹�Ƭ�����45��
* ���÷������������ͣ�24 
****************************************************************************/
u8 Func_Filter_Motor_Position(u8 dir,u8 speed,u8 index)
{
	u8 Response_CMD[3]={0x42,0x00,0xff};
	u8 ret =0;
	
	ret = Filter_Motor_Location(dir,speed,index);
	Response_CMD[1]=ret;
	USB_TxWrite(Response_CMD, 3);//������Ӧ

	return 0;
}


/****************************************************************************
* ��    �ƣ�u8 Func_Hardware_Test(void)
* ��    �ܣ�Ӳ�����Ժ���
* ��ڲ�����void
* ���ڲ��������ز��Ա�־
* ˵    ����������Ҫͨ���������Ӳ����ϵͳʱ��
* ���÷������������ͣ�80
****************************************************************************/
u8 Func_Hardware_Test(void)
{
	RCC_ClocksTypeDef  RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks); //��ȡ����ʱ��Ƶ��
	printf("SYSCLK_Frequency %d \r\n",RCC_Clocks.SYSCLK_Frequency);
	printf("HCLK_Frequency %d \r\n",RCC_Clocks.HCLK_Frequency);
	printf("PCLK1_Frequency %d \r\n",RCC_Clocks.PCLK1_Frequency);
	printf("PCLK2_Frequency %d \r\n",RCC_Clocks.PCLK2_Frequency);
	printf("ADCCLK_Frequency %d \r\n",RCC_Clocks.ADCCLK_Frequency);
	printf("---------------------------\r\n");
	return 0;
}


/****************************************************************************
* ��    �ƣ�u8 Func_CRC32_Test(void)
* ��    �ܣ�crc32���Ժ���
* ��ڲ�����void
* ���ڲ�����
* ˵    ����
* ���÷������������ͣ�81
****************************************************************************/
u8 Func_CRC32_Test(void)
{
	u8 i = 0;
	u8 s[6] = {1,2,3,4,5,6};
// 	make_crc32_table();
	for(i=0;i<6;i++)
	{
		s[i] = G_UART_Buffer[i];
		printf("s[%d] = %d\r\n",i,s[i]);
	}
  printf("%08Xh\r\n", make_crc(s, 6));
	return 0;
}



/****************************************************************************
* ��    �ƣ�u8 Func_Beep_Test(u8 delay)
* ��    �ܣ�����������
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷������������ͣ�82
****************************************************************************/
u8 Func_Beep_Test(u8 delay)
{
	u32 i =0;
	u32 delay_count =delay * 50;
	u8 Response_CMD[3]={0x28,0x00,0xff};
	
	for(i=0;i<5000;i++)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		delay_us(delay_count);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
    delay_us(delay_count);
	}
	
	Response_CMD[1] = delay;
	USB_TxWrite(Response_CMD, 3);//������Ӧ
	
	return 0;
}




/****************************************************************************
* ��    �ƣ�u8 Func_AD_Test(u8 mode,u8 wave,u8 channel,u8 filter_enable,u8 out_mode)
* ��    �ܣ�AD�ɼ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷������������ͣ�83
****************************************************************************/
u8 Func_AD_Test(u8 mode,u8 wave,u8 channel,u8 filter_enable,u8 out_mode)
{
// 	u16 adcx;
	float adcx;
	u32 temp;
	u16 temp1;
	u16 temp2;
	u8 i=0;
	u8 ret=0;
	float f1 = 0;
	u8 Response_CMD[3]={0x38,0x00,0xff};
	
	if((wave <0) || (wave >8) || (channel >7))
	{
		Response_CMD[1] = 1;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 1;
	}
	
	if(filter_enable==0)
	{
		ret = Filter_Motor_Location(1,9,wave);
		if(ret)
		{
			Response_CMD[1] = 2;
			USB_TxWrite(Response_CMD, 3);//������Ӧ
			return 2;
		}
	}
 	Adc_Init();		  		//ADC��ʼ��
	delay_ms(20);
	if(mode == 0)//����һ��ͨ��
	{
		
		adcx = Get_Adc_Average(channel,800);
		temp=adcx*3300/4096;
		
		temp1 = temp/1000;
		temp2 = temp%1000;

		if(out_mode == 0)
		{
			printf("wave=[%d]--channel=[%d]:%.4f ------%d mv-----%d.%03d mv\r\n",wave,channel,adcx,temp,temp1,temp2);
		}
		else if(out_mode == 1)
		{
			printf(":%.4f",adcx);
		}
		
		
// 		f1=(float)adcx*(3.3/4096);
// 		temp1 = (int)(f1/1);
// 		temp2 = (int)(f1*1000)%1000;
// 		printf("wave=[%d]--channel=[%d]:%d---%f mv --%d.%03d mv\r\n",wave,channel,adcx,f1,temp1,temp2);
	}
	else//������8ͨ��
	{
		for(i=0;i<8;i++)
		{
			adcx = Get_Adc_Average(i,500);
			temp=adcx*3300/4096;
			
			temp1 = temp/1000;
			temp2 = temp%1000;
			
			if(out_mode == 0)
			{
				printf("wave=[%d]--channel=[%d]:%.4f ------%4d mv-----%d.%03d mv\r\n",wave,i,adcx,temp,temp1,temp2);
			}
			else if(out_mode == 1)
			{
				printf(":%.4f",adcx);
			}
		}
	}
	
	if(filter_enable==0)
	{
		ret = Filter_Motor_Reset(1,9);
		if(ret)
		{
			Response_CMD[1] = 3;
			USB_TxWrite(Response_CMD, 3);//������Ӧ
			return 3;
		}
	}
// 	f1=f1*AD_K[channel];
// 	temp1 = (int)(f1/10);
// 	temp2 = (int)(f1*1000)%1000;
// 	printf("AD_Value_Adjust[%d]:------%f mv-----%d.%03d mv\r\n",channel,f1,temp1,temp2);
	

// 	printf("test:---- %6.3f --\r\n",3.14);
// 	printf("AD_Average[0][0]:---- %d --\r\n",AD_Average[0][0]);
// 	printf("AD_K:-%f--%f--%f--%f--%f--%f--%f--%f--\r\n",AD_K[0],AD_K[1],AD_K[2],AD_K[3],AD_K[4],AD_K[5],AD_K[6],AD_K[7]);
// 	printf("AD_K:-%d--%d--%d--%d--%d--%d--%d--%d--\r\n",AD_K[0],AD_K[1],AD_K[2],AD_K[3],AD_K[4],AD_K[5],AD_K[6],AD_K[7]);
	
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Plate_Motor_Position_Loop(u8 mode,u8 speed)
* ��    �ܣ�ͬ������� ѭ����λ ����
* ��ڲ�����step �����ж��ٲ�
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    �������һ��1.8�ȣ�200��һȦ
* ���÷������������ͣ�84
****************************************************************************/
u8 Func_Plate_Motor_Position_Loop(u8 mode,u8 speed)
{
	u8 Response_CMD[3]={0x91,0x00,0xff};
	u8 ret =0;
	
	if(speed == 0)
	{
		G_Step_Mode_Plus = 50;
	}
	else if(speed == 1)
	{
		G_Step_Mode_Plus = 100;
	}
	else if(speed == 2)
	{
		G_Step_Mode_Plus = 200;
	}

	ADC_Configuration(0);//ADͨ��ģʽ��ʼ��
	DMA_Configuration(0);
	
	ret = Filter_Plate_Motor_Loop(mode,G_Step_Mode_Plus);
	

	if(mode==0)
	{
		Process_AD_Value(0);//����ɼ���������
	}

	Printf_AD_Value_Average();

	Response_CMD[1]=ret;

	USB_TxWrite(Response_CMD, 3);//������Ӧ
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Wave_Sample(u8 wave_main,u8 wave_second,u8 col,u8 mode,u8 out_mode)
* ��    �ܣ���������
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷������������ͣ�85
****************************************************************************/
u8 Func_Wave_Sample(u8 wave_main,u8 wave_second,u8 col,u8 mode,u8 out_mode)
{
 	u8 Response_CMD[3]={0x58,0x00,0xff};
 	u8 wave_length[2]={0};
	u8 count =0;
	u8 filter_mode =0;
	u8 col_index =0;
// 	u16 *address = 0;
// 	u16 *address1 = 0;
	float *address = 0;
	float *address1 = 0;
	u8 i =0;
	u8 ret =0;
  
	
  //�������Ƿ���ȷ
	if((wave_main > 8) ||(wave_second > 8)||((wave_main==0)&&(wave_second > 0))||((wave_main==0)&&(wave_second==0)))
	{
		Response_CMD[1]=1;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 1;//��������
	}
	
	wave_length[0]=wave_main;
	wave_length[1]=wave_second;
  filter_mode = mode;
	col_index = col;
	
	if(wave_second)
	{
		Wavelength_Flag=1;//ʹ��˫��������
		count = 2;
	}
	else
	{
		Wavelength_Flag=0;//ʹ�õ���������
		count = 1;
	}
	
	//�򿪵�Դ
// 	G_Power_On_Off = 0;
// 	GPIO_SetBits(GPIOA,GPIO_Pin_8);	//����Դ��

	//��ʼ��AD
	Adc_Init();		
	
	//ѭ�����Σ��������ʹβ���
	for(i=0;i<count;i++)
	{
		if(filter_mode==0)
		{
			//�˹��ֶ�λ����Ӧ�Ĳ������� �� �� ��
			ret = Filter_Motor_Location(1,9,wave_length[i]);
			if(ret)//��λʧ��
			{
				Response_CMD[1]=ret;
				USB_TxWrite(Response_CMD, 3);//������Ӧ
				return 2;
			}
		}
		
		if (filter_mode==0)
		{
			//ø��帴λ
			ret = Plate_Motor_Reset(1);
			if(ret)//��λʧ��
			{
				Response_CMD[1]=ret;
				USB_TxWrite(Response_CMD, 3);//������Ӧ
				return 3;
			}
		}
		
		if(i==0)//������
		{
			address  = &AD_ALLlight_Single[0];//�����������µ�ֱ��ADֵ
			address1 = &AD_Halflight_Single[0][0];//�����������µ�12��8ͨ����ADֵ
		}
		else if(i==1)//�β���
		{
			address  = &AD_ALLlight_Double[0];//����β����µ�ֱ��ADֵ
			address1 = &AD_Halflight_Double[0][0];//����β����µ�12��8ͨ����ADֵ
		}
		
		if(filter_mode==0)
		{
			//�ɼ�8ͨ����Դֱ��ADֵ
			ADC0_Sample_Mode(address,2);
		}
		
		if(col_index == 13)//�� ȫ��12 ��
		{
			//��ʼһ��12��8ͨ���߰�AD�ɼ�
			AD_Loop_Sample(address1);
		}
		else//��ĳһ��
		{
			if(filter_mode==0)
			{
				if(col_index > 0)
				{
					ret = Plate_Motor_Position(1,col_index);
					if(ret)//ø��嶨λʧ��
					{
						Response_CMD[1]=ret;
						USB_TxWrite(Response_CMD, 3);//������Ӧ
						return 4;
					}
				}
			}
			
			
			if(i==0)//������
			{
				if(col_index==0)//�����
				{
					address1  = &AD_ALLlight_Single[0];//�����������µ�ֱ��ADֵ
				}
				else
				{
					address1 = &AD_Halflight_Single[col_index-1][0];//�����������µ�col_index��8ͨ����ADֵ
				}
			}
			else if(i==1)//�β���
			{
				if(col_index==0)//�����
				{
					address1  = &AD_ALLlight_Double[0];//����β����µ�ֱ��ADֵ
				}
				else
				{
					address1 = &AD_Halflight_Double[col_index-1][0];//����β����µ�col_index��8ͨ����ADֵ
				}
			}
			ADC0_Sample_Mode(address1,2);//�ɼ�8ͨ��
		}
	}
	
	//�رչ�Դ
// 	G_Power_On_Off = 1;
// 	GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//�رչ�Դ��

	if(filter_mode==0)
	{
		//�˹��ָ�λ
		ret = Filter_Motor_Reset(1,9);
		if(ret)//��λʧ��
		{
			Response_CMD[1]=ret;
			USB_TxWrite(Response_CMD, 3);//������Ӧ
			return 4;
		}
	}
	
	ret = Plate_Motor_Reset(1);
	if(ret)//��λʧ��
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 3;
	}

	
// 	//��ȡϵͳ����8��������Ӧ�� K,B,T
// 	Flash_Read_K_B_T(120);	//��K,B,T
// 	Flash_Read_Std_Target(122);	//����ֵ��
	
	
	//�ɼ���ADֵ֮�󣬼���ABS
	Data_Handling(wave_main,wave_second);
	
	//���ɼ�����96�����ݷ��ͳ�ȥ
	USB_TxWrite(Response_CMD, 3);//������Ӧ
	USB_Out_Sample_Abs(Wavelength_Flag,out_mode);
	
	//�����־
	Wavelength_Flag = 0;
	
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_AD_K_B_Calibrate(u8 mode,u8 type,u8 wave,u8 std,u8 out_mode)
* ��    �ܣ�//���� - AD 8ͨ��-����Ȳ���-���ܵ���������8ͨ����4����׼���ʵ�K,Bֵ
* ��ڲ���mode��0��ABCDͨ�� , 1��EFGHͨ��   2��ABCDEFGͨ��
* ��ڲ���type��0����·���ܼ���,  1��Һ����������
* ��ڲ���wave������
* ��ڲ���std����ֵ
* ��ڲ���out_mode��0����ϸ���-��������Ա��,  1��ֻ�����Ч����
* ���ڲ�����
* ˵    ����
* ���÷������������ͣ�86
****************************************************************************/
u8 Func_AD_K_B_Calibrate(u8 mode,u8 type,u8 wave,u8 std,u8 out_mode)
{
 	u8 Response_CMD[3]={0x68,0x00,0xff};
	u8 ret =0;
  u8 wave_length = wave;//�ܹ�4�ֲ���
	//u8 i = 0;
  SelectCalib = std;
	Flash_Read_Std_Target(123);	//����ֵ��

	if((type!=0)&&(type!=1))
	{
		Response_CMD[1]=1;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 1;
	}
	
	//�򿪹�Դ
	G_Power_On_Off = 0;
	GPIO_SetBits(GPIOA,GPIO_Pin_8);	//����Դ��

	//ADC��ʼ��
	Adc_Init();	

	//�˹��ֶ�λ��ָ���Ĳ���
	ret = Filter_Motor_Location(1,9,wave_length);
	if(ret)//��λʧ��
	{
 		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 2;
	}
	//��ʼѭ��10���߰�ɼ���ѹ��
	//AD_Alllight[10][8] ��� 8ͨ��ֱ��AD
	//AD_Halflight[10][24] ��� 24�ױ�׼���ʵ�AD
	ret = AD_Loop_Sample_K_B(mode,type);
	if(ret) 
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 3;
	}

	
	if(out_mode == 0)
	{
		USB_Out_K_B_Abs(1,wave_length);	//��ӡ���---������Ա����	
	}
	
	if(ret == 0)
	{
		//����10��16�׵�ABS��ƽ��ֵ
		//ABS_Array[10][16]
		//ABS_Aver_Array[16]
		Data_Handling_Optical(mode,type);
	}
	
	if(out_mode == 0)
	{
		USB_Out_K_B_Abs(2,wave_length);//��ӡ���---������Ա����
	}	
	
	
	if(type==0)//0����·���ܼ���
	{
		//��ʼ���㵱ǰ�����µ� K,B
		X_Y_Kb(wave_length,mode);
	}
	else if(type==1)//1��Һ����������
	{
		//��ʼ������ǰ�����µ������
		Data_Handling_LiquidSamples(wave_length);
	}
	
	
	if(out_mode == 0)
	{
		//ͨ��USB�˿ڷ���
		USB_Out_K_B_Abs(3,wave_length);//��ӡ���---������Ա����
	}	
	
	//��֯Ҫ���͵�����
	if(type==0)//0����·���ܼ���
	{
		if(out_mode == 1)
		{
			Array_Combination_Optical(wave_length,mode);
			//ͨ��USB�˿ڷ���
			USB_Out_K_B_Abs(4,wave_length);//��ӡ���---ֻ�����Ч����
		}
	}
	else if(type==1)//1��Һ����������
	{
		if(out_mode == 1)
		{
 			//ͨ��USB�˿ڷ���
			//�˹��ܿ���ֱ���ڲ����н���-
			USB_Out_K_B_Abs(5,wave_length);//��ӡ���---ֻ�����Ч����
		}
	}
	
// 	if(type==0)
// 	{
// 		//�������õ���K,B,T
// 		Flash_Write_K_B_T(120);
// 	}

	//�رչ�Դ
	G_Power_On_Off = 1;
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//�رչ�Դ��

	//�˹��ָ�λ
	ret = Filter_Motor_Reset(1,9);
	if(ret)//��λʧ��
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
		return 4;
	}
	return 0;
}



/****************************************************************************
* ��    �ƣ�void Func_Plate_Run_Step(u8 mode,u8 plus)
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷������������ͣ�87
****************************************************************************/
void Func_Plate_Run_Step(u8 mode,u8 plus)
{
	u8 Response_CMD[3]={0x78,0x00,0xff};

  u16 count = 200;


	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//ʹ�ܵ��
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);//�������
	
	if(mode == 0)//���Ե�Ƭ �� ��λ����ľ���
	{
		//�ö�λƬ�������
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//���ס��1�� ͨ��Ϊ0
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			count = count - 1;
		}
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		printf("count = %d\r\n",count);
	}
	else if(mode == 1)//����ø��� �� ��ʼ �� �����г̾���
	{
		count = 600+plus;
		while((count > 0))
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			count = count - 1;
		}
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
	}
	USB_TxWrite(Response_CMD, 3);//������Ӧ

}


/****************************************************************************
* ��    �ƣ�u8 Func_Device_Self_Test(void)
* ��    �ܣ�����Լ�
* ��ڲ�����
* ���ڲ���������ֵ��8λ�ֱ��ʾ��
*					��7��
*					��6��
*					��5��
*					��4��
*					��3���˹��ָ�λʧ��
*					��2�����غ�ADģ�����
*					��1���˹��ֶ�λ����
*					��0��ø������
* ˵    �����˺�����Ҫ���ڻ�û�����ӵ���ʱ�����������Լ�����������Գ����ж������Ƿ�����
* ���÷����������������ֻ����һ����    -88
****************************************************************************/
u8 Func_Device_Self_Test(void)
{
	u8 Response_CMD[3]={0x88,0x00,0xff};
	u8 ret0 = 0;
	u8 ret = 0;
	u16 i = 0;
	u16 ad_value = 0;
	
	//����--------------------------------------------------------
	G_Power_On_Off = 0;//�ƹ�������� �� �ر� ��Դ
  GPIO_SetBits(GPIOA,GPIO_Pin_8);	//����Դ��, ȫ��

	//���������� 1�룬 ���������Ƿ�����
	for(i=0;i<2000;i++)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		delay_us(200);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
    delay_us(200);
	}
	
	//ø��嶨λ----------------------------------------------------
 	ret = Plate_Motor_Position(1,12);
	ret0 += ret;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//������Ӧ

 	delay_ms(10);
	//ø��帴λ
	ret = Plate_Motor_Reset(1);
	ret0 += ret;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//������Ӧ
	if(ret)//��λʧ�ܣ���������һ��
	{
		for(i=0;i<1500;i++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
	}
	delay_ms(10);
	
	ret = 0; 
	//�˹��ֶ�λ�� 1���˹�Ƭ-------------------------------------------
 	ret = Filter_Motor_Location(1,9,1);
	ret0 += ret;
	
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//������Ӧ
	if(ret)//�˹�Ƭ��λʧ�ܣ���������һ��
	{
		for(i=0;i<1500;i++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
	}
	delay_ms(20);
	
// 	ret = 0; 
// 	//����8ͨ���ɼ�������ѹ�����������غ�ADģ���Ƿ�����-------------
//  	Adc_Init();		  		//ADC��ʼ��
// 	for(i=0;i<8;i++)
// 	{
// 		ad_value = Get_Adc_Average(i,10);
// 		if(ad_value<50)//���߹���--�ο�660ȡֵ
// 		{
// 			ret = 1; 
// 			ret0 = ret0 | 0x04; 
// 			break;
// 		}
// 	}
// 	Response_CMD[0]=4;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 2);//������Ӧ
	
// 	if(ret)//��ѹ���ʧ�ܣ���������һ��
// 	{
// 		for(i=0;i<1500;i++)
// 		{
// 			GPIO_SetBits(GPIOD,GPIO_Pin_2);
// 			delay_us(250);
// 			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
// 			delay_us(250);
// 		}
// 	}

	delay_ms(10);
	ret = 0; 	
	//�˹��ָ�λ-----------------------------------------------------------
  ret = Filter_Motor_Reset(1,9);
	ret0 += ret;
// 	Response_CMD[1]=ret;
// 	USB_TxWrite(Response_CMD, 3);//������Ӧ
	if(ret)//��λʧ�ܣ���������һ��
	{
		for(i=0;i<1500;i++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
	}
	delay_ms(10);
	
	Response_CMD[1] = ret0;
	USB_TxWrite(Response_CMD, 3);//������Ӧ
	
	//�ص�------------------------------------------------------------------
  GPIO_ResetBits(GPIOA,GPIO_Pin_8);	  //�ع�Դ��
	
	//�ڵƹ���������п�����Դ,
	G_Power_On_Off = 1;//�ƹ�������� �� ���� ��Դ�� ΢��
	G_Power_On_Off_Fre = 1;
	return ret0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Std_Target_Test(u8 mode,u8 index)
* ��    �ܣ�����-���ձ�׼���ʵİ�ֵ
* ��ڲ�����
* ���ڲ�����
* ˵    ������ֵ����� 123 ������
* ���÷�����-89
****************************************************************************/
u8 Func_Std_Target_Test(u8 mode,u8 index)
{
	u8 Response_CMD[3]={0x98,0x00,0xff};
	u8 i =0;
	u8 try_count = 0;
	u8 *std;
	
	
	switch(index)
	{
		case 1:
			std = (u8 *)&targetValueA[0];
			break;
		case 2:
			std = (u8 *)&targetValueB[0];
			break;
		case 3:
			std = (u8 *)&targetValueC[0];
			break;
		default:
			return 1;
			break;
	}
	
	
	if(mode==0)//д��ֵ
	{ 
		while(try_count < 10)
		{
			//usb������ 11�ֽڵ����� �� 96�ֽڵİ�ֵ
			if(G_USB_Buffer_Size < 107)
			{
				delay_ms(100);
				try_count += 1;
			}
			else
			{
				break;
			}
		}
		
		//���ȴ�1000�����ֱ�Ӵ�USB������ȡ��96�ֽڵ����ݣ�������ʱ�����������Ƿ�������
		
		for(i=0;i<96;i++)
		{
			std[i] = G_USB_Buffer[11+i];
		}
		
		Flash_Write_Std_Target(123);
		Response_CMD[1]=0;
		USB_TxWrite(Response_CMD, 3);//������Ӧ
	}
	else if(mode ==1)//����ֵ
	{
		Flash_Read_Std_Target(123);	//����ֵ��
		
		for(i=0;i<96;i++)
		{
	// 		printf("targetValue%d[%d]=%d\r\n",index,i,std[i]);
			printf(":%d",std[i]);
		}		
	}
}

/****************************************************************************
* ��    �ƣ�u8 Func_Flash_Write_Read_Test(u8 mode,u8 wave)
* ��    �ܣ�KBT����
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����-90
****************************************************************************/
u8 Func_Flash_Write_Read_Test(u8 mode,u8 wave)
{
	u8 Response_CMD[3]={0x09,0x00,0xff};
	u8 i=0;
	u8 j=0;
	u16 *addr=0;
	float *addr_array_k;
	float *addr_array_b;
	float *addr_array_t;
	
// 	switch(wave)
// 	{
// 		case 1:
// 			addr_array_k = (float*)&Slope_405nm[0][0];
// 			addr_array_b = (float*)&Intercept_405nm[0][0];
// 			addr_array_t = (float*)&TestValue_405nm[0][0];
// 			break;
// 		case 2:
// 			addr_array_k = (float*)&Slope_450nm[0][0];
// 			addr_array_b = (float*)&Intercept_450nm[0][0];
// 			addr_array_t = (float*)&TestValue_450nm[0][0];
// 			break;
// 		case 3:
// 			addr_array_k = (float*)&Slope_492nm[0][0];
// 			addr_array_b = (float*)&Intercept_492nm[0][0];
// 			addr_array_t = (float*)&TestValue_492nm[0][0];
// 			break;
// 		case 4:
// 			addr_array_k = (float*)&Slope_630nm[0][0];
// 			addr_array_b = (float*)&Intercept_630nm[0][0];
// 			addr_array_t = (float*)&TestValue_630nm[0][0];
// 			break;
// 		default:
// 			return 1;
// 			break;
// 	}
	
	if(mode == 0)//����д
	{
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_k = 1.1234;
// 			addr_array_k = addr_array_k +1;
// 		}
// 		
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_b = 2.3456;
// 			addr_array_b = addr_array_b +1;
// 		}

// 		for(i=0;i<24;i++)
// 		{
// 			*addr_array_t = 3.4567;
// 			addr_array_t = addr_array_t +1;
// 		}
		
		Flash_Write_K_B_T(120);
		
// 		//����K,B,Tʱ�� ˳�� �Ѱ�ֵ��д��flash
// 		Flash_Write_Std_Target(122);
		
		USB_TxWrite(Response_CMD, 3);//������Ӧ
	}
	else if(mode ==1)//���Զ�
	{
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_k = 0;
// 			addr_array_k = addr_array_k +1;
// 		}
// 		
// 		for(i=0;i<32;i++)
// 		{
// 			*addr_array_b = 0;
// 			addr_array_b = addr_array_b +1;
// 		}

// 		for(i=0;i<24;i++)
// 		{
// 			*addr_array_t = 0;
// 			addr_array_t = addr_array_t +1;
// 		}
		
		Flash_Read_K_B_T(120);
	
		switch(wave)
		{
			case 1:
				addr_array_k = (float*)&Slope_405nm[0][0];
				addr_array_b = (float*)&Intercept_405nm[0][0];
				addr_array_t = (float*)&TestValue_405nm[0][0];
				break;
			case 2:
				addr_array_k = (float*)&Slope_450nm[0][0];
				addr_array_b = (float*)&Intercept_450nm[0][0];
				addr_array_t = (float*)&TestValue_450nm[0][0];
				break;
			case 3:
				addr_array_k = (float*)&Slope_492nm[0][0];
				addr_array_b = (float*)&Intercept_492nm[0][0];
				addr_array_t = (float*)&TestValue_492nm[0][0];
				break;
			case 4:
				addr_array_k = (float*)&Slope_630nm[0][0];
				addr_array_b = (float*)&Intercept_630nm[0][0];
				addr_array_t = (float*)&TestValue_630nm[0][0];
				break;
			default:
				break;
		}
		
		
		//ȡ�� K,B,T	
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
// 				printf("Slope_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_k );
				printf(":%0.4f",*addr_array_k );
				addr_array_k = addr_array_k +1;
			}
		}
			
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
// 				printf("Intercept_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_b);
				printf(":%0.4f",*addr_array_b );
				addr_array_b = addr_array_b +1;
			}
		}

		for(i=0;i<8;i++)
		{
			for(j=0;j<5;j++)
			{
// 				printf("TestValue_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_t);
				printf(":%0.4f",*addr_array_t );
				addr_array_t = addr_array_t +1;
			}
		}
	}
		
	return 0;
}

/****************************************************************************
* ��    �ƣ�u8 Func_Get_Temperature(u8 index)
* ��    �ܣ���ȡ�¶�
* ��ڲ�����index ���¶ȴ���������
* ���ڲ��������ظ�λ�Ƿ�ɹ���־ 0���ɹ� 1��ʧ��
* ˵    ����
* ���÷�����
****************************************************************************/
// float Func_Get_Temperature(u8 index)
// {
// 	u8 * promid = NULL;
// 	float wd=0;

// 	switch(index)
// 	{
// 		case 0:
// 			promid = DS18B20_ID01;
// 			break;
// 		case 1:
// 			promid = DS18B20_ID02;
// 			break;
// 		case 2:
// 			promid = DS18B20_ID03;
// 			break;
// 		case 3:
// 			promid = DS18B20_ID04;
// 			break;
// 		default:
// 			break;
// 	}
// 	wd = DS18B20_Get_Temp(promid);
// 	return wd;
// }

/****************************************************************************
* ��    �ƣ�u8 Func_CRC32_Cmd_Check(u8* src, u8 len)
* ��    �ܣ�crc32ָ����֤
* ��ڲ�����void
* ���ڲ�����
* ˵    ����
* ���÷�����
****************************************************************************/
u8 Func_CRC32_Cmd_Check(u8* src, u8 len)
{
	u32 crc_result =0;
	
	
	//�жϻ����������Ƿ����������һ��ָ��11���ֽ�
	if(len >= 11)
	{
		if(*(src+10)!=0xff)
		{
			return 1;
		}
		return 0;
		//�ж�ָ���Ƿ���Ч
		crc_result = make_crc(src,6);//����CRC32У����
		printf("crc_result = %d\r\n",crc_result);
		printf("%d = %d\r\n",src[6],crc_result>>24);
		printf("%d = %d\r\n",src[7],(crc_result&0x00ff0000)>>16);
		printf("%d = %d\r\n",src[8],(crc_result&0x0000ff00)>>8);
		printf("%d = %d\r\n",src[9],(crc_result&0x000000ff));
		
		if((src[6]==crc_result>>24)&&(src[7]==(crc_result&0x00ff0000)>>16)&&(src[8]==(crc_result&0x0000ff00)>>8)&&(src[9]==(crc_result&0x000000ff)))
		{
			return 0;//ָ����Ч
		}
		else
		{
			return 1;//ָ����Ч
		}
	}
	else
	{
			return 2;//ָ���
	}
}

