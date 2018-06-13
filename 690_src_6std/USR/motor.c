/****************************************************************************
* �ļ����ƣ�motor.c
* ˵    ��������ļ�
* ��    �ܣ�������Ҫ��ͬ������� �� �˹��ֵ����������
* ���÷���1��uint8_t Plate_Motor_Reset(void)  // ͬ���������λ
* ���÷���2��uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse)//ͬ��������˶����ٸ�����
* ���÷���3��
* ���÷���4��
* ���÷���5��
* ���÷���6��uint8_t Filter_Motor_Location(uint8_t dir,uint8_t position) //�˹��ֵ����λ
* ���÷���7��uint8_t Filter_Motor_Reset(void) //�˹��ֵ����λ
* ���÷���8��
* ���÷���9��
* ���÷���10��
* ���÷���11��
****************************************************************************/
#include "motor.h"
#include "ad.h"

int8_t step_index = 0;//�˹���4��8�ĵ�����
int8_t plate_motor_stop_flag = 0;//ͬ�������ֹͣ��־
int8_t filter_motor_stop_flag = 0;//ͬ�������ֹͣ��־



//----------------------------------------------������ͬ���������غ���---------------------------------------------------------------//

/****************************************************************************
* ��    �ƣ�uint8_t Plate_Motor_Reset(u8 speed)
* ��    �ܣ�ͬ���������λ
* ��ڲ���1����
* ���ڲ�����0:��λ�ɹ���
* ���ڲ�����1:��λʧ�ܣ�ͬ������λ�������
* ���ڲ�����2:��λʧ�ܣ�����������Ӳ������
* ���ڲ�����3:��λʧ�ܣ�Ӳ������
* ˵    ������λ��Ƭ��������г�48��
* ���÷�����ע��ø��帴λ ����ֻ��Ҫ�õ������������Ϳ����ˣ�����д����΢���ӵ㣬�Ѷ�λ����Ҳ���ǽ�����
*           ��Ƭ�뿪��λ����︴λ����һ���������������Ը�������ϵ�����õĿ��Ƹ�λ����Ҫ���뾡�����ϵͳ���ȶ��ԡ�
*           ���ַ��� ���� ���� ��ʱ �������������ϣ�ø��帴λ��ʱ�򲻻�һֱ�˶�����ʱ���Խ�����λ������ֹͣø�������˶�
****************************************************************************/
uint8_t Plate_Motor_Reset(u8 speed)
{
	uint8_t speed_delay = 0;
	uint16_t pluse_count = 0;
	uint16_t position_opt_count = 0;//��λ�������������
	
	speed_delay = speed;
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//ʹ�ܵ��

	//���ȼ���Ƿ��Ѿ�λ�ڸ�λ��
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0)//��������������  ����ΪPin_5
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		return 0;//�Ѿ����ڸ�λ�㣬����0
	}
	
	//���ڸ�λ�㣬��ʱ��������������λ�ڸ�λ�����������
	//���Կ�������ͬ����������ַ�������60�����岢��⣬��ⲻ��Ȼ���ٷ��������ж�
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);//�������
	pluse_count = 100;//��Ƭ ���붨λ����� �г�Ϊ 52������
	//ѭ���˳�����������⵽��λ�㣬 ��λ��Ƭ�Ѿ�����������������
// 	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(pluse_count > 0))
	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(pluse_count > 0))  //����ΪPin_5
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 		delay_ms(1);
		delay_ms(speed_delay);
		GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 		delay_ms(1);
		delay_ms(speed_delay);
		pluse_count = pluse_count - 1;
	}
	delay_ms(20);

	//�ж��Ƿ��Ѿ�λ�ڸ�λ��
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0)//�������������� , ����ΪPin_5
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		return 0;//�Ѿ����ڸ�λ�㣬����0
	}
	
	if(pluse_count == 0)//ͬ������λ�������
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		G_Error_Type |= 1<<2; //��¼�쳣״̬
		return 1;
	}
	
	//��Ƭ�Ѿ����붨λ����
	if ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0))//��λ��Ƭ�Ѿ�������
	{
		//û�ҵ���λ�㣬�����˶�
		GPIO_SetBits(GPIOB,GPIO_Pin_1);//���ַ���
		pluse_count = 605;
// 		while((pluse_count > 0))
 		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != 0)&&(pluse_count > 0))  //�������������� , ����ΪPin_5
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
	// 		delay_ms(1);
			delay_ms(speed_delay);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
	// 		delay_ms(1);
			delay_ms(speed_delay);
			pluse_count = pluse_count - 1;
			
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)//�жϵ�Ƭ�Ƿ��Ѿ��뿪��λ����
			{
				position_opt_count =position_opt_count +1;
				if(position_opt_count > 50)
				{
					break;
				}
			}
			else
			{
				position_opt_count = 0;
			}
		}
		
		//��Ƭ�뿪��λ��������������ﵽ48�� ��ʱ�ܿ����ǻ���������Ӳ������
		if(position_opt_count > 50)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			G_Error_Type |= 1<<3; //��¼�쳣״̬
			return 2;//��λʧ��
		}
		
		//605���������꣬��û�ҵ����϶�Ӳ������,��ʱͬ����������ϵĿ����ԱȽϴ�
		if(pluse_count==0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			G_Error_Type |= 1<<4; //��¼�쳣״̬
			return 3;//��λʧ��
		}
		else
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			G_Error_Type &= ~((1<<2)|(1<<3)|(1<<4)); //����쳣״̬
			return 0;//��λ�ɹ�
		}
	}
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
	G_Error_Type &= ~((1<<2)|(1<<3)|(1<<4)); //����쳣״̬
	return 0;//��λ�ɹ�
}

/****************************************************************************
* ��    �ƣ�uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse)
* ��    �ܣ�ͬ�����������ָ����������
* ��ڲ���1������ 
* ��ڲ���1���ٶ�    
* ��ڲ���1��������  
* ���ڲ�����0: �ɹ���������ָ����������
* ���ڲ�����1: �ɹ����������˸�λ��
* ���ڲ�����2: ʧ�ܣ������������
* ˵    ������� 200 �� һȦ  ��ͬ�������г� 605������
* ���÷������˺��� �����˶���������λ���ֹͣ�� �����˶���ʱһֱ���е�ָ����������������ָ����pluse��Ҫ̫�󣬷����һֱ���е��ײ�
****************************************************************************/
uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse)
{
		uint16_t pluse_count = 0;
	  uint32_t speed_count = 0;
	
  	speed_count = speed *100;
 		pluse_count = pluse;
	  GPIO_ResetBits(GPIOB,GPIO_Pin_0);//ʹ�ܵ��
		if(dir)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_1);//��������-����
		}
		else
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_1);//������ -����
		}
		
		//1.����ǳ��ַ��򣬶����Ѿ����е���λ���ˣ��Ͳ��ڼ���ת����
		if((dir==1)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0))  ////�������������� , ����ΪPin_5
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			return 1;//����pluse_count�����壬�������˸�λ�㣬Ҳ����
		}
		
		//2.��ʼִ��pluse������
		if(dir==1)//����
		{
			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != 0)&&(pluse_count > 0))  //�������������� , ����ΪPin_5
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				pluse_count = pluse_count - 1;
			}			
			
			if(pluse_count==0)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
				return 0;//ִ����pluse_count������
			}
			else
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
				return 1;//����pluse_count�����壬���Ѿ��ﵽ��λ��
			}
		}
		else if(dir==0)//����
		{
// 			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) != 0)&&(pluse_count > 0))
			while((pluse_count > 0))
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_us(speed_count);
				pluse_count = pluse_count - 1;
			}

			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			return 0;//ִ����pluse_count������
			
		}
		else
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			return 2;//�����������
		}
}

/****************************************************************************
* ��    �ƣ�uint8_t Plate_Motor_shake(uint8_t dir,uint8_t speed,uint8_t time)
* ��    �ܣ�ͬ�����������ָ���������� ��
* ��ڲ���1������ 
* ��ڲ���1���ٶ�    
* ��ڲ���1��ʱ��
* ���ڲ�����0:�ɹ�
* ˵    ��������ָ�Ӹ�λ�㵽���ؼ��ķ���
* ���÷������˺��� �����˶���������λ���ֹͣ�� �����˶���ʱһֱ���е�ָ����������
****************************************************************************/
uint8_t Plate_Motor_Shake(uint8_t speed,uint8_t time)
{
		uint16_t pluse_count = 20;
	  uint8_t speed_count =0;
	  uint8_t time_count =0;
	  uint8_t dir = 1;
		u8 ret = 0;

    speed_count = speed;
		time_count  = time;
		
		ret = Plate_Motor_Reset(1);//ø����ȸ�λ
		
	
   	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//ʹ�ܵ��
		//ø��� ����һ�ξ���������
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);//�������
		while((pluse_count > 0))
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			pluse_count = pluse_count - 1;
		}	
		
		pluse_count = 10;
		while(time_count > 0)
		{
			//2.��ʼִ��pluse������
			if(dir==1)//����
			{
				while((pluse_count > 0))
				{
					GPIO_ResetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					GPIO_SetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					pluse_count = pluse_count - 1;
				}	
				dir = 0;				
			}
			else if(dir==0)//����
			{
				while(pluse_count > 0)
				{
					GPIO_ResetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					GPIO_SetBits(GPIOB,GPIO_Pin_2);
					delay_ms(speed_count);
					pluse_count = pluse_count - 1;
				}
				dir = 1;				
			}
			time_count = time_count -1;
			pluse_count = 10;
			if(dir)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_1);//�������
			}
			else
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_1);//�������
			}
		}			
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		return ret;
}

/****************************************************************************
* ��    �ƣ�uint8_t Plate_Motor_Position(uint8_t speed,uint8_t index)
* ��    �ܣ�ͬ���������λ
* ��ڲ���1����λ�ٶ�
* ��ڲ���2����λ����
* ���ڲ�����0:��λ�ɹ���
*         ��1:��λʧ�ܣ���������
*         ��2:��λʧ�ܣ���λʧ��
*         ��3:��λʧ�ܣ�ͬ������λ�����
*         ��4:��λʧ�ܣ�Ӳ������
* ˵    ����ø����г� �ܴ�Լ605��
* ���÷�����ÿ�ζ�λ���Ը�λ��Ϊ�ο���
****************************************************************************/
uint8_t Plate_Motor_Position(uint8_t speed,uint8_t index)
{
	uint16_t pluse_Max = 600;
	uint16_t count = 100;
	uint8_t speed_delay = speed;
	uint8_t col = index;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t ret = 0;
	
	if(col <= 0 || col > 12 || speed <=0)
	{
		return 1;//��������
	}
	
 	ret = Plate_Motor_Reset(1);
	if(ret == 0)//��λ�ɹ�
	{
		delay_ms(20);//
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);//ʹ�ܵ��
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);//�������
		
		//�ö�λƬ�������
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//���ס��1�� ͨ��Ϊ0
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
			delay_ms(speed_delay);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
			delay_ms(speed_delay);
			pluse_Max = pluse_Max - 1;
			count = count - 1;
		}
		
		if(count==0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
			G_Error_Type |= 1<<2; //��¼�쳣״̬
			return 3;//ͬ������λ�����
		}
		
		//��ʽ��ʼ��λ
		for(i=0;i<col;i++)
		{
			//����7������ʽΪ���õ�Ƭ�ƶ�һ����룬�ö�λ��ƫ������������һ����λ��
			for(j=0;j<7;j++)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
				delay_ms(speed_delay);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
				delay_ms(speed_delay);
				pluse_Max = pluse_Max - 1;
			}
			
 			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//���ס��1�� ͨ��Ϊ0
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
// 			delay_ms(1);
				delay_ms(speed_delay);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
// 				delay_ms(1);
				delay_ms(speed_delay);
				pluse_Max = pluse_Max - 1;
			}
			
			if(pluse_Max == 0)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
				G_Error_Type |= 1<<4; //��¼�쳣״̬
				return 4;//��λʧ��
			}
		}
	}
	else//��λʧ��
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		return 2;
	}
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���	
	return 0;//��λ�ɹ�
}
//----------------------------------------------�������˹�����غ���---------------------------------------------------------------//

/****************************************************************************
* ��    �ƣ�void Filter_Motor_Run(uint8_t dir)
* ��    �ܣ��˹��ֵ���˶�
* ��ڲ���1������ �� 1������ 0������
* ���ڲ�������
* ˵    ��������1�� ��400��1Ȧ
* ���÷�����		
****************************************************************************/
void Filter_Motor_Run(uint8_t dir)
{
	if (filter_motor_stop_flag == 1)
	{
		return;
	}
	
	switch(step_index)
	{
		case 0://1000
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 1://1100
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 2://0100
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 3://0110
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 4://0010
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			break;
		case 5://0011
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			break;
		case 6://0001
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			break;
		case 7://1001
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			break;
	}
	
	if(dir)//����
	{
		step_index = step_index+1;
		if(step_index > 7)
		{
			step_index=0;
		}
	}
	else//����
	{
		step_index = step_index-1;
		if(step_index < 0)
		{
			step_index=7;
		}
	}
}

/****************************************************************************
* ��    �ƣ�uint8_t Filter_Motor_Reset(uint8_t dir,uint8_t speed)
* ��    �ܣ��˹��ֵ����λ
* ��ڲ�����
* ���ڲ���: 0����λ�ɹ��� 1����λʧ�� 
* ˵    ����
* ���÷�����400 ������һȦ 
****************************************************************************/
uint8_t Filter_Motor_Reset(uint8_t dir,uint8_t speed)
{
	uint16_t step_count = 500;//���Ĳ��Բ�����
	uint8_t speed_delay = speed;
	
	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) != 0)&&(step_count > 0))
	{
		Filter_Motor_Run(dir);//�˹��ֵ������һ��
// 		delay_ms(2);//��ʱ2ms
		delay_ms(speed_delay);
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		step_count = step_count -1;
	}
	
	if(step_count==0)
	{
		G_Error_Type |= 1<<5;
		return 1;//��λʧ��
	}
	else
	{
		G_Error_Type &= ~(1<<5);
		return 0;//��λ�ɹ�
	}
}

/****************************************************************************
* ��    �ƣ�uint8_t Filter_Motor_Location(uint8_t dir,uint8_t speed,uint8_t position)
* ��    �ܣ��˹��ֵ����λ
* ��ڲ���1��dir���� 1������ ��0:����
* ��ڲ���2���ٶ�
* ��ڲ���3��positionλ�ã��˹�Ƭλ�ã�0����λ��1��405nm, 2:450nm, 3:492nm, 4:630nm
* ���ڲ���: 0����λ�ɹ��� 1����λʧ�� ��2��position��������
* ˵    ����
* ���÷�����400 ������һȦ 
****************************************************************************/
uint8_t Filter_Motor_Location(uint8_t dir,uint8_t speed,uint8_t position)
{
	uint16_t step_count = 0;//
	uint16_t speed_delay = speed;//
	uint8_t error =0;
	//1.�˹����ȸ�λ
	error = Filter_Motor_Reset(dir,speed_delay);
	if(error)
	{
		return 1;//�˹��ָ�λʧ��
	}
	//2.ȷ����λ������
	switch(position)
	{
		case 0://��λ��
			step_count = 0;
			break;
		case 1://405nm
			step_count = 50;
			break;
		case 2://450nm
			step_count = 100;
			break;
		case 3://492nm
			step_count = 150;
			break;
		case 4://630nm
			step_count = 200;
			break;
		case 5://δʹ��
			step_count = 250;
			break;
		case 6://δʹ��
			step_count = 300;
			break;
		case 7://δʹ��
			step_count = 350;
			break;
		case 8://δʹ��
			step_count = 400;
			break;
		default:
			break;
	}
	//3.��λ
	while(step_count > 0)
	{
		Filter_Motor_Run(dir);//�˹��ֵ������һ��
// 		delay_ms(2);//��ʱ2ms
		delay_ms(speed_delay);//
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);

		step_count = step_count -1;
	}
	return 0;
}

//----------------------------------------------������ͬ����������˹��ֵ�� ��Ϲ���---------------------------------------------------------------//
/****************************************************************************
* ��    �ƣ�uint8_t Filter_Plate_Motor_Loop(u8 mode,u8 speed)
* ��    �ܣ���ѯ��λ
* ��ڲ���1��dir���� 1������ ��0:����
* ��ڲ���2��positionλ�ã��˹�Ƭλ�ã�0����λ��1��405nm, 2:450nm, 3:492nm, 4:630nm
* ���ڲ���: 0����λ�ɹ��� 1����λʧ�� ��2��position��������
* ˵    ����
* ���÷�����
****************************************************************************/
 uint8_t Filter_Plate_Motor_Loop(u8 mode,u8 speed)
{
	uint8_t ret = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t wave_length = 1;
	uint16_t pluse_Max = 600;
	uint8_t count = 100;
// 	u8 Response_CMD[2]={0x28,0xff};
	
	ret = Filter_Motor_Location(1,5,wave_length);
	if(ret)//��λʧ��
	{
		return 1;
	}
	
	//ø��帴λ
	ret = Plate_Motor_Reset(1);
	if(ret)//��λʧ��
	{
		return 1;
	}
		// 		//�˹��ֶ�λ�� 1�Ų���
	// 		wave_length = i%8;
	// 		Filter_Motor_Location(1,3,wave_length);

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
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
		return 2;//ͬ������λ����������
	}	
		
	if(mode==0)//����
	{
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
				return 3;//��λʧ��
			}

			//��λ��1��
			AD_Sample_Mode1(i);
	// 			Get_Adc_Mode0(i);
		
		}
	}
	else//����
	{
		//��ʽ��ʼ��λ12��
		for(i=0;i<12;i++)
		{
			//����7������ʽΪ���õ�Ƭ�ƶ�һ����룬�ö�λ��ƫ������������һ����λ��
			for(j=0;j<7;j++)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				pluse_Max = pluse_Max - 1;
			}
			
			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//���ס��1�� ͨ��Ϊ0
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(2);
				pluse_Max = pluse_Max - 1;
			}
			
			if(pluse_Max == 0)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
				return 3;//��λʧ��
			}

			//��λ��1��
// 			AD_Sample_Mode1(i);
	 			Get_Adc_Mode0(i,speed);
		
		}
	}
	
	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//��ֹ���
	delay_ms(2);

	//��λ
	ret = Plate_Motor_Reset(1);
	if(ret)//��λʧ��
	{
		return 1;
	}
	
	ret = Filter_Motor_Reset(1,5);
	if(ret)//��λʧ��
	{
		return 4;
	}
	
	return 0;
}

