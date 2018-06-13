/****************************************************************************
* �ļ����ƣ�main.c
* ˵    ��: ���ļ�
* ��    �ܣ�������Ҫ�����߼�
* ���÷���1��void main(void)	                    ����ļ�
* ���÷���2��void start_task(void *pdata)         ��ʼ����
* ���÷���3��void MAIN_task(void *pdata)          ������
* ���÷���4��
* ���÷���5��
* ���÷���6��
* ���÷���7��
* ���÷���8��
* ���÷���9��
* ���÷���10��
* ���÷���11��
****************************************************************************/

#include "pbdata.h"
#include "uart.h"
#include "ad.h"
#include "crc32.h"
#include "func.h"
#include "usb_config.h"
#include "ds18b20.h"
#include "motor.h"
#include "heat.h"
#include "flash.h"

//ucosII�����ջ����
//�����������ȼ��� ���ȼ�����Ҫ����Ҫ��ʱ������������Ҫ���ø����ȼ�
#define START_TASK_PRIO 	12
#define TEMP_TASK_PRIO   	9
#define BEEP_TASK_PRIO   	8
#define LAMP_TASK_PRIO 	  7
#define AD_TASK_PRIO 		  6
#define FUNC_TASK_PRIO 		5
#define UART_TASK_PRIO 	  4
#define USB_TASK_PRIO 		3


//���������ջ��С
#define START_STK_SIZE   128 // �ռ��С=64*4(�ֽ�)
#define AD_STK_SIZE      128 // �ռ��С=64*4(�ֽ�)
#define UART_STK_SIZE    128 // �ռ��С=64*4(�ֽ�)
#define USB_STK_SIZE  	 512 // �ռ��С=64*4(�ֽ�)
#define FUNC_STK_SIZE  	 512 // �ռ��С=64*4(�ֽ�)
#define TEMP_STK_SIZE  	 128 // �ռ��С=64*4(�ֽ�)
#define BEEP_STK_SIZE  	 128 // �ռ��С=64*4(�ֽ�)
#define LAMP_STK_SIZE  	 128 // �ռ��С=64*4(�ֽ�)

//���������ջ�ռ�
//ʹ�� Microlib ���printf �Ⱥ����Ը������Ĵ�����ַ�Ԥ��ֵ�� ��ʱ�������ʹ�õĶ�ջǰ8�ֽڶ���__align(8)
__align(8) OS_STK START_TASK_STK[START_STK_SIZE];
__align(8) OS_STK AD_TASK_STK[AD_STK_SIZE];
__align(8) OS_STK UART_TASK_STK[UART_STK_SIZE];
__align(8) OS_STK USB_TASK_STK[USB_STK_SIZE];
__align(8) OS_STK FUNC_TASK_STK[FUNC_STK_SIZE];
__align(8) OS_STK TEMP_TASK_STK[TEMP_STK_SIZE];
__align(8) OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
__align(8) OS_STK LAMP_TASK_STK[LAMP_STK_SIZE];


//��������
void start_task(void *pdata);
void USB_task(void *pdata);
void UART_task(void *pdata);
void FUNC_task(void *pdata);
void Ad_task(void *pdata);
void BEEP_task(void *pdata);
void TEMP_task(void *pdata);
void LAMP_task3(void *pdata);



/****************************************************************************
* ��    �ƣ�void TEMP_task(void)
* ��    �ܣ�TEMP_task��������
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ������������
* ���÷���:
****************************************************************************/
void TEMP_task(void *pdata)
{
	u8 Response_CMD[3]={0x21,0x00,0xff};
	u16 temp;
	float wd;
	u8 count= 0;
	
	while(1)
	{
		count += 1; 
		//����λ�����͵�ǰDS18B20 ��⵽���¶�
		if(G_Temperature_Disp == 1 && count > 5)
		{
			count = 0;
			temp=DS18B20_Get_wd(1);//���Ȱ��ڵ��¶�
			if((temp&0xF800)==0xF800)//���¶��ж�
			{
				temp=~temp;
				temp=temp+1;
				wd=temp*(-0.0625);
			}
			else
			{
				wd=temp*0.0625;	
			}
			if (wd < 80)
			{
				printf(":%.1f:",wd);
			}
// 			printf("%d %f",Target_Temperature ,wd);
			delay_ms(1000);
		}
		
		if(G_Heat3_Start == 1)   //����Ƭ����
		{
			//G_Beep_on_off = 1;
			Func_Temperature_Heat(Target_Temperature);  //�ú�����Ҫһ������
			//G_Beep_on_off = 0;
			delay_ms(1000);
		}
		else if(G_Heat3_Start == 2)//����-������
		{
			Heat3_Control(Target_Temperature,IDLESTATE);
			delay_ms(1000);
		}

    if(G_Heat1_Start==1)    //����������
		{
			//G_Beep_on_off = 1;			
			Func_Peltier_Board_Heat(Target_Temperature);
			//G_Beep_on_off = 0;
			delay_ms(1000);
		}
		else if(G_Heat1_Start == 2)//����-������
		{
			Heat1_Control(Target_Temperature,IDLESTATE);
			delay_ms(1000);
		}
		
		if(G_Heat2_Start==1)    //����������
		{
			//G_Beep_on_off = 1;			
			Func_Peltier_Board_Cold(Target_Temperature);
			//G_Beep_on_off = 0;
			delay_ms(1000);
		}
		else if(G_Heat2_Start == 2)//����-������
		{
			Heat2_Control(Target_Temperature,IDLESTATE);
			delay_ms(1000);
		}
		
		
		delay_ms(500);
	}
}

/****************************************************************************
* ��    �ƣ�void LAMP_task(void)
* ��    �ܣ�����±�صƴ������ȹ�������
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ��������������
* ���÷���:
****************************************************************************/
void LAMP_task(void *pdata)
{
	while(1)
	{
		if(G_Power_On_Off)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_8);	//����Դ��
			delay_ms(2*G_Power_On_Off_Fre);
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);	//�رչ�Դ��
			delay_ms(1);
		}
		delay_ms(10);
	}
}

/****************************************************************************
* ��    �ƣ�void BEEP_task(void)
* ��    �ܣ�BEEP_task��������
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ��������������
* ���÷���:
****************************************************************************/
void BEEP_task(void *pdata)
{
// 	uint8_t err;
	uint8_t i=0;
	
	while(1)
	{
//     printf("---BEEP_task02---\r\n");
		if(G_Beep_on_off)
		{
			for(i=0;i<50;i++)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
	// 			delay_us(100*beep_count);
				delay_us(100);
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);
				delay_us(100);
	// 			delay_us(100*beep_count);
			}
			delay_ms(10);
		}
		else
		{
			delay_ms(20);
		}

	}
}

/****************************************************************************
* ��    �ƣ�void Ad_task(void)
* ��    �ܣ�AD��������
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ������ͨ���� ����ת��ģʽ �� DMA��ɨ��
* ���÷������� 
****************************************************************************/
void Ad_task(void *pdata)
{
//   uint32_t len = 0;
// 	u8 error = 0;
//   uint8_t buf[200] = {0};
	u8 i=0;
	pdata=pdata;

	while(1)
	{		
// 		printf("---Ad_task---\r\n");

		if (AD_MODE ==0)
		{
			if(i==12)//�ܹ�12��
			{
				Process_AD_Value(AD_MODE);//����ɼ���������
				Printf_AD_Value_Average();
// 				Printf_AD_Value(AD_MODE);
				Clear_AD_Value(AD_MODE);
				delay_ms(20);
				i=0;
				AD_MODE = 3;
			}			
			AD_Sample_Mode1(i);
			i++;
		}
		else if(AD_MODE ==1)
		{
			if(i==12)//�ܹ�12��
			{
				Process_AD_Value(AD_MODE);//����ɼ���������
				Printf_AD_Value_Average();
// 				Printf_AD_Value(AD_MODE);
				Clear_AD_Value(AD_MODE);
				delay_ms(20);
				i=0;
				AD_MODE = 3;
			}
			AD_Sample_Mode2(i);
			i++;
		}
		else
		{
			delay_ms(20);
		}
	}
}

/****************************************************************************
* ��    �ƣ�void FUNC_task(void)
* ��    �ܣ�FUNC_task��������
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ���������񱻶���Ӧ���ֿ��������
* ���÷���:
****************************************************************************/
void FUNC_task(void *pdata)
{
	u8 Response_CMD[3]={0x00,0x00,0xff};
	uint8_t err;
	uint8_t i=0;
	u16 temp;
	OS_CPU_SR cpu_sr=0;
	
	while(1)
	{
//printf("---FUNC_task---\r\n");

		//�ȴ�ָ���źŵĵ���
		G_Func_Running = 0;
		delay_ms(10);
		OSSemPend(Func_Sem,0,&err);
		//���յ��ź�
		G_Func_Running = 1;
		
		switch(G_Func_Type)
		{
			case FUNC_TYPE_CONNECT: //�������  03
				G_Beep_on_off = 1;
				Func_Front_Connect_Test();
				delay_ms(5);				
				G_Beep_on_off = 0;
				break;
			case FUNC_TYPE_SELF_TEST: //�����Լ� 04
				G_Beep_on_off = 1;
				Func_Front_Self_Test();
				delay_ms(5);				
				G_Beep_on_off = 0;
				break;
			case FUNC_TYPE_PREPARE_TEST: //׼������
				Func_Sample_Test_Prepare();
				break;
			case FUNC_TYPE_START_TEST: //��ʼ����
				Func_Sample_Test_Start();
				break;
			case FUNC_TYPE_STOP_TEST: //����ֹͣ
				Func_Sample_Test_Stop();
				break;
			case FUNC_TYPE_COM_OPEN: //�򿪴���
				Func_Com_On();
				break;
			case FUNC_TYPE_COM_CLOSE: //�رմ���
				Func_Com_Off();
				break;
			case FUNC_TYPE_USB_OPEN: //��USB�˿�
				Func_USB_On();
				break;
			case FUNC_TYPE_USB_CLOSE: //�ر�USB�˿�
				Func_USB_Off();
				break;
			case FUNC_TYPE_LIGHT_ON: //�򿪹�Դ    0D
				G_Power_On_Off = 0;
			  Func_Light_On(); //ȫ��
				break; 
			case FUNC_TYPE_LIGHT_OFF: //�رչ�Դ   0E
				//G_USB_Buffer[1]�� �ƹ����� Ƶ�� ��Сֵ 1
				Func_Light_Off(G_USB_Buffer[1]);
				break;
///////////////////////////////////////////////////////////////////				
			case FUNC_TYPE_TEMPERATURE_READ: //�¿� 12
				if (G_USB_Buffer[1]==0)
				{
					G_Temperature_Disp = 1;//���͵�ǰ�¶�����
				}
				else if (G_USB_Buffer[1]==1)
				{
					G_Temperature_Disp = 0;//�����͵�ǰ�¶�����
				}
				else if (G_USB_Buffer[1]==2)
				{
					//����״̬ --ͬʱ���͵�ǰ�¶�����
					G_Temperature_Disp = 1;
					Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //Ŀ���¶ȼ���
					Func_Temperature_Hot(Target_Temperature);  

				}
				else if (G_USB_Buffer[1]==3)
				{
					//����״̬--ͬʱ���͵�ǰ�¶�����
					G_Temperature_Disp = 1;
					Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //Ŀ���¶ȼ���
					Func_Temperature_Cold(Target_Temperature);  
				}
				else if(G_USB_Buffer[1]==4)
				{
					//����״̬--����Ƭ���������ڹ�������ʱ��û�йرգ����PWM���壬��ռ�ձ�Ϊ0--��ʱ����Ƭ-������������
					G_Temperature_Disp = 0;//
					Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //Ŀ���¶ȼ���
					Func_Temperature_Idle(Target_Temperature);
				}
				else if(G_USB_Buffer[1]==5)
				{
					//�ر�״̬--����Ƭ���������ڹ�������ʱ��Ҳ���ڲ���PWM����
					G_Temperature_Disp = 0;//
					Func_Temperature_Off();
				}
				break;
			case FUNC_TYPE_TEMPERATURE_ON: //����Ƭ���� 13
				Heat3_Init();
			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //Ŀ���¶ȼ���
				G_Heat3_Start =1;//��ʼ����
			  G_Temperature_Disp = 2;//����Ƭ�����¶�����
				break;
			case FUNC_TYPE_TEMPERATURE_OFF: //����Ƭ�ر�  14
				//����Ƭ ������ ���ͬʱ�ڹ����� ���߶��õ��˶�ʱ���� ���Թرռ���Ƭ��ʱ���ܹرն�ʱ����ֻ������һ�����ڴ���״̬
				if(G_Heat1_Start !=1 && G_Heat2_Start!=1)
				{
					G_Temperature_Disp = 0;
					Func_Temperature_Off();
				}
				else
				{
					G_Temperature_Disp = 3;
					G_Heat3_Start =2;//�رռ���
				}
				Response_CMD[0]=0x41;
				Response_CMD[1]=0x00;
			  G_Temperature_Disp = 0;
				USB_TxWrite(Response_CMD, 3);//������Ӧ
				break;
			case FUNC_TYPE_TEMPERATURE_HOT: //��������  15
				
			  G_Temperature_Disp = 3;//���������¶�����
				//�ȶ�ȡ����ģ����¶�
				i=0;
				while(i<100)
				{
					temp=DS18B20_Get_wd(2);
					temp = Temperature_convert(temp);
					if (temp >10 && temp < 700)//��һ�ζ��¶Ⱦ�������850
					{
						break;
					}
					delay_ms(10);
				}
				
			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //Ŀ���¶ȼ���
// 				printf("%d - %d\r\n",temp,Target_Temperature);
				if(temp < Target_Temperature)//˵���趨���¶ȸ��ڻ����¶�--����
				{
					Peltier_Init(1);
					G_Heat1_Start =1;//��ʼ����
					G_Heat2_Start =0; 
				}
				else if(temp > Target_Temperature)//˵���趨���¶�С�ڻ����¶�--����
				{
					Peltier_Init(3);
					G_Heat2_Start =1;              //��ʼ����
					G_Heat1_Start =0;
				}
				
				break;
			case FUNC_TYPE_TEMPERATURE_COLD: //�����ر� 16
				if(G_Heat3_Start != 1)
				{
					G_Temperature_Disp = 0;
					Func_Temperature_Off();
				}
				else if(G_Heat1_Start == 1)
				{
					G_Temperature_Disp = 2;
					G_Heat1_Start = 2;
				}				
				else if(G_Heat2_Start == 1)
				{
					G_Temperature_Disp = 2;
					G_Heat2_Start =2; 
				}
				Response_CMD[0]=0x61;
				Response_CMD[1]=0x00;
				USB_TxWrite(Response_CMD, 3);//������Ӧ
				break;
// 			case FUNC_TYPE_TEMPERATURE_HOT: //��������   15
// 				
// 			  Peltier_Init(1);
// 			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //Ŀ���¶ȼ���
// 			  G_Heat1_Start =1;//��ʼ����
// 				break;
// 			case FUNC_TYPE_TEMPERATURE_COLD: //��������  16
// 				Peltier_Init(3);
// 			  Target_Temperature = G_USB_Buffer[2]*10+G_USB_Buffer[3];  //Ŀ���¶ȼ���
// 			  G_Heat2_Start =1;              //��ʼ����
// 				break;
			
///////////////////////////////////////////////////////////////////			
			case FUNC_TYPE_PLATE_RESET: //ø��帴λ  17
				//G_USB_Buffer[1]�� �ٶ�    �ο�ֵ 1
 				Func_Plate_Motor_Reset(G_USB_Buffer[1]);
				break;
			case FUNC_TYPE_PLATE_RUN: //ø�������,��������ø���Ĳ���   18
				//G_USB_Buffer[1]�� �ٶ�    �ο�ֵ 10
				//G_USB_Buffer[2]�� ����    
				Func_Plate_Motor_Run(G_USB_Buffer[1],G_USB_Buffer[2]);
				delay_ms(500);
				break;
			case FUNC_TYPE_PLATE_POSITION: //ø��嶨λ   19
				//G_USB_Buffer[1]�� �ٶ�
				//G_USB_Buffer[2]�� ������
  			Func_Plate_Motor_Position(G_USB_Buffer[1],G_USB_Buffer[2]);
				break;
			case FUNC_TYPE_PLATE_SHAKE: //ø������   1A
				//G_USB_Buffer[1]�� ���ǿ��
				//G_USB_Buffer[2]�� ���ʱ��
				Func_Plate_Motor_Shake(G_USB_Buffer[1],G_USB_Buffer[2]);
				delay_ms(500);
				break;
			case FUNC_TYPE_ABS_TEST: //ABS����    1B
				Func_ABS_Test();
				AD_MODE = 0;
 				ADC_Configuration(AD_MODE);//ADͨ��ģʽ��ʼ��
				DMA_Configuration(AD_MODE);
				break;
			case FUNC_TYPE_ABS_STOP: //ABSֹͣ    1C
				Func_ABS_Stop();
				AD_MODE = 3;
				break;
			case FUNC_TYPE_LUMINOUS_TEST: //��ǿAD����  1D
				Func_LM_AD_Test();
				AD_MODE = 1;
 				ADC_Configuration(AD_MODE);//ADͨ��ģʽ��ʼ��
				DMA_Configuration(AD_MODE);
				break;
			case FUNC_TYPE_LUMINOUS_STOP: //��ǿADֹͣ  1E
				Func_LM_AD_Stop();
				AD_MODE = 3;
				break;
			case FUNC_TYPE_FILTER_RESET: //�˹��ָ�λ    23
				//G_USB_Buffer[1]�� ����
				//G_USB_Buffer[2]�� �ٶ�  �ο�ֵ 8
				Func_Filter_Motor_Reset(G_USB_Buffer[1],G_USB_Buffer[2]);
				delay_ms(500);
				break;
			case FUNC_TYPE_FILTER_POSITION: //�˹��ֶ�λ  24
				//G_USB_Buffer[1]�� ����
				//G_USB_Buffer[2]�� �ٶ�   �ο�ֵ 9
				//G_USB_Buffer[3]�� ���������Է�8���˹�Ƭ����� 0-7
				Func_Filter_Motor_Position(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3]);
			  delay_ms(500);
				break;
			case 0x80://���� - Ӳ��ʱ��
 				Func_Hardware_Test();
				break;
			case 0x81://���� - crc32У��ֵ
 				Func_CRC32_Test();
				break;
			case 0x82://���� - ������
				//G_USB_Buffer[1]�� ��������ʱ
				OS_ENTER_CRITICAL();//�����ٽ������ж��޷����
 				Func_Beep_Test(G_USB_Buffer[1]);
				OS_EXIT_CRITICAL()//�˳��ٽ��� ���Ա��жϴ��
				break;
			case 0x83://���� - AD ����ͨ��
				G_Temperature_Disp = 0;//������ʱ�򣬲�Ҫ�����¶�����
 				//G_USB_Buffer[1]�� 0������ͨ�� ��1��������8ͨ��
 				//G_USB_Buffer[2]�� �ڼ�����
 				//G_USB_Buffer[3]�� �ڼ�ͨ�� ,����ͨ������Ҫ�˲���
 				//G_USB_Buffer[4]�� �Ƿ��Զ������˹�Ƭ��λ ��0������  1��������
 				//G_USB_Buffer[5]�� ����ϸ������ ���� ��������
				Func_AD_Test(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3],G_USB_Buffer[4],G_USB_Buffer[5]);
				break;
			case 0x84://���� - ø��岽��ģʽ
 				//G_USB_Buffer[1]������ģʽ������
 				//G_USB_Buffer[2]�������ٶȣ� �ߣ��У���
				Func_Plate_Motor_Position_Loop(G_USB_Buffer[1],G_USB_Buffer[2]);
				break;
			case 0x85://���� - ˫��������
				G_Temperature_Disp = 0;//������ʱ�򣬲�Ҫ�����¶�����
				//G_USB_Buffer[1]�� ������ 
 				//G_USB_Buffer[2]�� �β���
 				//G_USB_Buffer[3]�� �ڼ��У� 0���ޣ�1����һ�� ... 13:����12��
 				//G_USB_Buffer[4]�� �Ƿ����˹����Զ���λ 0������ ��1��������
 				//G_USB_Buffer[5]�� �Ƿ���ϸ����������� 0����ϸ��� ��1��ֻ������
				Func_Wave_Sample(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3],G_USB_Buffer[4],G_USB_Buffer[5]);
				break;
			case 0x86://���� - AD 8ͨ��-����Ȳ���-���ܵ���������8ͨ����6����׼���ʵ�K,Bֵ
				G_Temperature_Disp = 0;//������ʱ�򣬲�Ҫ�����¶�����
 				//G_USB_Buffer[1]�� 0��ABCDͨ�� ,     1��EFGHͨ��  
 				//G_USB_Buffer[2]�� 0����·���ܼ���,  1��Һ���������� (��ʱ����Ҫ��һ������G_USB_Buffer[1])
 				//G_USB_Buffer[3]�� ����
 				//G_USB_Buffer[4]�� ʹ���ĸ���׼���ʵİ�ֵ
 				//G_USB_Buffer[5]�� 0����ϸ�����������,  1��ֻ�����Ч����
				Func_AD_K_B_Calibrate(G_USB_Buffer[1],G_USB_Buffer[2],G_USB_Buffer[3],G_USB_Buffer[4],G_USB_Buffer[5]);
				break;
			case 0x87://����ø������е��г�
				//G_USB_Buffer[1] 0������ø��嵽 ��λ������г̣� 1�����������г�
				//G_USB_Buffer[2]
				Func_Plate_Run_Step(G_USB_Buffer[1],G_USB_Buffer[2]);
				break;
			case 0x88://���� - ø����Լ�
				Func_Device_Self_Test();
				break;
			case 0x89://���� - ���ձ�׼���ʵİ�ֵ
				G_Temperature_Disp = 0;//������ʱ�򣬲�Ҫ�����¶�����
				//G_USB_Buffer[1] :��ʾ ��д  0��дflash�� 1����flash   
				//G_USB_Buffer[2] :����
				Func_Std_Target_Test(G_USB_Buffer[1],G_USB_Buffer[2]);				
				break;
			case 0x90://���� - K��B��Tֵ�Ķ�д
				G_Temperature_Disp = 0;//������ʱ�򣬲�Ҫ�����¶�����
				//G_USB_Buffer[1] :��ʾ ��д  0��дflash�� 1����flash   
				//G_USB_Buffer[2] :����
				Func_Flash_Write_Read_Test(G_USB_Buffer[1],G_USB_Buffer[2]);				
				break;
			default:
				break;
		}
		
// 		USB_TxWrite(G_USB_Buffer, G_USB_Buffer_Size);//���Խ��յ�������
		// ���USBָ���
		//USB�ӿ�һ���յ�11�ֽڵ�ָ��ͻᴥ����Ӧ�Ĺ���ִ�У���ʱUSB�������濪ͷ��ŵ���11�ֽ�ָ������ŵ�����λ���������͵�ָ�����ݡ�
		//��ǰָ��ִ����Ϻ󣬼�ʹ�ڴ��ڼ����յ���ָ����ݣ�һ�ɲ����������USB���棬���¿�ʼ�������ݡ�
		//����Ҫ��ÿ������ܵõ�����һ��Ҫ��������������Ϊֹ��
 		USB_Buffer_CMD_Clear();//���ָ�����
	}
}


/****************************************************************************
* ��    �ƣ�void UART_task(void)
* ��    �ܣ����ڽ�����������
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ���� 
* ���÷���:��������Ҫ���մ��ڷ��͹��������ݣ� ��Ҫ������ǹ
****************************************************************************/
void UART_task(void *pdata)
{
	u8 error = 0;
// 	u8 i = 0;
	u8 Response_CMD[3]={0x30,0x00,0xee};

	pdata=pdata;
	while(1)
	{
//  printf("---UART_task---\r\n");
		
		if(G_UART_Buffer_Size>0)//���������յ�����
		{
			if(G_Computer_Connect_Flag==0)//������û������ ��λ�� �� ��ʱ�����յ������� ������ ��λ�����͵ļ�⴮���Ƿ���ڵ�ָ��
			{
				//�ж��յ���ָ���Ƿ���ȷ
				error = Func_CRC32_Cmd_Check(G_UART_Buffer,G_UART_Buffer_Size);
				if(error == 2)//ָ���
				{	
					delay_ms(20);
					continue;
				}
				else if(error ==1)//ָ����Ч
				{
					UART_Buffer_CMD_Clear();//���ָ�����
					continue;
				}
				else if(error ==0)//ָ����Ч�����浱ǰָ��
				{
					if(G_UART_Buffer[0] == 0x03)//����ָ����Զ˿ڴ治����
					{
						//3.ָ���
						UART_Buffer_CMD_Send1(Response_CMD,2);
						UART_Buffer_CMD_Clear();//���ָ�����
						delay_ms(20);
					}
				}
			}
			else if(G_Computer_Connect_Flag==1)//�����Ѿ�����λ��ͨѶ���ӳɹ�
			{
				
				
				UART_Buffer_CMD_Send1(G_UART_Buffer,G_UART_Buffer_Size);
				UART_Buffer_CMD_Clear();//���ָ�����
				
				delay_ms(20);
			}	
		}
		else
		{
			delay_ms(20);
		}
	}
}


/****************************************************************************
* ��    �ƣ�void USB_task(void)
* ��    �ܣ�USB ����
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ������������մ����жϲ��������Ϊ���ܿ�����Ӧ�������͹�����������������ȼ�Ҫ����������Ҫ�ߣ�
*           �������������ȼ���������������񣬵����������͹���������ò�����ʱ��Ӧ����ʱ�����������ܿ��������Ĺ���
*           ��Σ���λ����Ҫ�����������������λ��ֻ��ʶ�����һ��������淢�����������ֱ������
* ���÷������������� USB �˿ڵĽ��գ� USB�˿ڽ��տ�������
****************************************************************************/
void USB_task(void *pdata)
{
  uint32_t len = 0;
	u8 error = 0;
  uint8_t buf[100] = {0};

	//DS18B20��ʼ��
  error = DS18B20_Configuration(1);
// 	printf("error1 = %d\r\n",error);
	G_Error_Type |= error<<0;
  error = DS18B20_Configuration(2);
// 	printf("error2 = %d\r\n",error);
	G_Error_Type |= error<<1;
	
  //����Լ�
	Func_Device_Self_Test();
	
  //��ȡϵͳ����8��������Ӧ�� K,B,T
	Flash_Read_K_B_T(120);	//��K,B,T

	//ע�⣺������¼��������汣���У������ �� ��ֵ�� ���� ��Ч�ģ�
  //1.������ȷ����ֵ�� ����ڲ�����һ��Ĭ�ϵİ�ֵ�� ���Ҫʹ���µİ�ֵ�������뽫�µİ�ֵ���浽������flash�С�
  //2.�����Ƚ��й�����У����д����ȷ��У������


	while(1)
	{	
		//��USB���ڽ�������
		len = USB_RxRead(buf, sizeof(buf));
		if (len > 0)
		{
			//���浽usb���ڻ���
			Copy_To_Usb_Buf(buf,len,G_USB_Buffer);
			
			if(G_Func_Running==1)//FUNC����������ִ�У�˵��֮ǰ�����û��ִ����ϣ���������ݵ�����ֱ�ӱ����ڻ�����
			{
				delay_ms(20);
				continue;
			}

			//����յ���ָ�������Ƿ���ȷ
			error = Func_CRC32_Cmd_Check(G_USB_Buffer,G_USB_Buffer_Size);
			if(error == 2)//ָ���
			{
				delay_ms(20);
				continue;
			}
			else if(error == 1)//ָ�����
			{
				USB_Buffer_CMD_Clear();//���USB����
				continue;
			}
			else if(error ==0)//ָ����Ч
			{	
// 				//�Ѿ��յ�11�����ݣ������ʱֻ�ǵ�����������治������,���ѳ�������Ĳ��ֶ��겢����
// 				if(G_USB_Buffer[0]!=0x89)//0x89 ���հ�ֵ���������а�ֵ������
// 				{
// 					do
// 					{
// 						//��USB�����е����ݶ��꣬������Ҫ�Ƿ�ֹ���û����������˶�����ֻ�ѵ�һ��������Ϊ��Ч�����������ȫ��������֤ÿ�ε�������һ��ָ��
// 						len = USB_RxRead(buf, sizeof(buf));
// 					}while(len>0);
// 				}

				//����ָ���¼ǰһ��ָ��͵�ǰָ������¼ǰ������ָ���Ҫ����Ϊ�Ժ���׼��������ǰ��ָ����صĹ��ܡ�
				if((G_Pre_Cmd_Type == G_Curr_Cmd_Type))//ǰ������ָ����ͬ
				{
					G_Curr_Cmd_Type = G_USB_Buffer[0];//���µ�ǰָ��
				}
				else//ǰ������ָ���ͬ
				{
					G_Pre_Cmd_Type  = G_Curr_Cmd_Type;// ����ָ��
					G_Curr_Cmd_Type = G_USB_Buffer[0];// ����ָ��
				}
				
				//ָ���
				G_Func_Type = G_Curr_Cmd_Type;
// 				USB_TxWrite(G_USB_Buffer, G_USB_Buffer_Size);//���Խ��յ�������
				if(G_Func_Running==0)//FUNC����������
				{
					OSSemPost(Func_Sem);
				}				
			}
		}
		else
		{
			delay_ms(20);
		}
	}
}


/****************************************************************************
* ��    �ƣ�void start_task(void)
* ��    �ܣ���ʼ����
* ��ڲ�����void *pdata
* ���ڲ�������
* ˵    ������������ϵͳ�ĵ�һ������ֻ���𴴽���������Ĺ�����
* ���÷������� 
****************************************************************************/
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;

	pdata=pdata;
	
	Func_Sem = OSSemCreate(0);

	OSStatInit();//��ʼ��ͳ������

	OS_ENTER_CRITICAL();//�����ٽ������ж��޷����
	OSTaskCreate(USB_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&USB_TASK_STK[USB_STK_SIZE-1],//����������ջ��ջ��ָ��
								USB_TASK_PRIO);//�������������ȼ�
	
	OSTaskCreate(UART_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&UART_TASK_STK[UART_STK_SIZE-1],//����������ջ��ջ��ָ��
								UART_TASK_PRIO);//�������������ȼ�
								
	OSTaskCreate(Ad_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&AD_TASK_STK[AD_STK_SIZE-1],//����������ջ��ջ��ָ��
								AD_TASK_PRIO);//�������������ȼ�

	OSTaskCreate(FUNC_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&FUNC_TASK_STK[FUNC_STK_SIZE-1],//����������ջ��ջ��ָ��
								FUNC_TASK_PRIO);//�������������ȼ�
								
	OSTaskCreate(TEMP_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&TEMP_TASK_STK[TEMP_STK_SIZE-1],//����������ջ��ջ��ָ��
								TEMP_TASK_PRIO);//�������������ȼ�
								
	OSTaskCreate(BEEP_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&BEEP_TASK_STK[BEEP_STK_SIZE-1],//����������ջ��ջ��ָ��
								BEEP_TASK_PRIO);//�������������ȼ�
								
	OSTaskCreate(LAMP_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&LAMP_TASK_STK[LAMP_STK_SIZE-1],//����������ջ��ջ��ָ��
								LAMP_TASK_PRIO);//�������������ȼ�

 	OSTaskSuspend(START_TASK_PRIO);//����ʼ����
	OS_EXIT_CRITICAL()//�˳��ٽ��� ���Ա��жϴ��
	
}


/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ����������
* ��ڲ�������
* ���ڲ�������
* ˵    ������main����֮ǰ���Ѿ�����SystemInit()��������ϵͳʱ�����ú����£�
*           SysClk =72MHZ  �� =PLLCLK  ,=�ⲿ����(8MHZ) * PLL��Ƶϵ��PLLMUL=9
*						AHBCLK =72MHZ  �� =SysClk/AHB   , ��Ӧ HCLK
*						APB1CLK=36MHZ  �� =AHBCLK/APB1  , ��Ӧ PCLK1
*						APB2CLK=72MHZ  �� =AHBCLK/APB2  , ��Ӧ PCLK2
*					  ϵͳ��ʱ��SysTickʱ��Ƶ�� = 9MHZ , =AHBCLK/8
* ���÷������� 
****************************************************************************/
int main(void)
{
	//1.ʱ�ӳ�ʼ��
	RCC_Configuration();//ϵͳʱ�ӳ�ʼ��
	//2.�˿�GPIO��ʼ��
	GPIO_Configuration();//�˿ڳ�ʼ��
	//3.ϵͳ�δ�ʱ�ӳ�ʼ��
 	Delay_Init();//ucosiiʱϵͳ��ʱ����ʼ��
// 	NVIC_Configuration(NVIC_PriorityGroup_2,SysTick_IRQn,0,0); //SysTick���жϷ���2����ռ���ȼ�0����Ӧ���ȼ�0
	//4.����1��ʼ��
  USART_Configuration(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No);//����1��������9600�� 8������λ��1��ֹͣλ����У��λ
  NVIC_Configuration(NVIC_PriorityGroup_2,USART1_IRQn,1,1); //����1���жϷ���2����ռ���ȼ�1����Ӧ���ȼ�1
	
	//Pwm_Init();
	
	//AD1��ʼ��
// 	ADC_Configuration(AD_MODE);//AD��ͨ��ģʽ��ʼ��
//  	Adc_Init();		  		//ADC��ʼ��
	
	//DMA��ʼ��
	DMA_Configuration(AD_MODE);
	
	//USB��ʼ��
 	USB_Configuration();//USB��ʼ��
	
	//DS18B20��ʼ��
//   DS18B20_Configuration();

	//����CRC32	У�����
	make_crc32_table();

	//UCOS��ʼ��
	OSInit();//��ʼ��UCOS����ϵͳ
	//������ʼ����
	OSTaskCreate(start_task,//ָ����������ָ��
								(void *)0,//����ʼִ��ʱ�����ݸ����������ָ��
								(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],//����������ջ��ջ��ָ��
								START_TASK_PRIO);//�������������ȼ�
	//����ucos
	OSStart();//����ucos����ϵͳ
}


