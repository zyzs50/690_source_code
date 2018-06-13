#ifndef __FUNC_H__
#define __FUNC_H__
#include "pbdata.h"

extern u8 Func_Front_Connect_Test(void);//�������� -03
extern u8 Func_Front_Self_Test(void);//ǰ�˰��Լ� -04
extern u8 Func_Sample_Test_Prepare(void);//׼����Ʒ���� -06
extern u8 Func_Sample_Test_Start(void);//��ʼ�������� -07
extern u8 Func_Sample_Test_Stop(void);//�˳��������� -08
extern u8 Func_Com_On(void);//���ڿ��� - 09
extern u8 Func_Com_Off(void);//���ڹر� - 0A
extern u8 Func_USB_On(void);//USB���� - 0B
extern u8 Func_USB_Off(void);//USB�ر� - 0C
extern u8 Func_Light_On(void);//��Դ�ر� - 0D
extern u8 Func_Light_Off(u8 fre);//��Դ�ر� - 0E
extern u8 Func_Temperature_Disp(void);//������ʾ - 13
extern u8 Func_Temperature_Heat(u16 Target);
extern u8 Func_Temperature_Off(void);//�����ر� - 14
extern u8 Func_Peltier_Board_Heat(u16 Target);//���� - 15
extern u8 Func_Peltier_Board_Cold(u16 Target);//���� - 16
extern u8 Func_Plate_Motor_Reset(u8 speed);//ø��帴λ - 17
extern u8 Func_Plate_Motor_Run(uint16_t speed,uint16_t step);//ø����߰� - 18
extern u8 Func_Plate_Motor_Position(u8 speed,u8 index);//ø��嶨λ - 19
extern u8 Func_Plate_Motor_Shake(u8 speed,u8 time);//ͬ��������� -	1A
extern u8 Func_ABS_Test(void);//ABS��ֵ - 1B
extern u8 Func_ABS_Stop(void);//ABSֹͣ - 1C
extern u8 Func_LM_AD_Test(void);//��ǿֹͣ - 1D
extern u8 Func_LM_AD_Stop(void);//��Դ�ر� - 1E
extern u8 Func_Filter_Motor_Reset(u8 dir,u8 speed);//�˹��ָ�λ - 23
extern u8 Func_Filter_Motor_Position(u8 dir, u8 speed, u8 index);//�˹��ֶ�λ - 24



extern u8 Func_Hardware_Test(void);//Ӳ������-��������ʱ��  - 80
extern u8 Func_CRC32_Test(void);//crc32У��ֵ - 81
extern u8 Func_Beep_Test(u8 delay);//��������֤ - 82
extern u8 Func_AD_Test(u8 mode,u8 wave,u8 channel,u8 filter_enable,u8 out_mode);//ADͨ������ - 83
extern u8 Func_Plate_Motor_Position_Loop(u8 mode,u8 speed);//ø���ѭ����λ���� - 84
extern u8 Func_Wave_Sample(u8 wave_main,u8 wave_second,u8 col,u8 mode,u8 out_mode);//˫�������� - 85
extern u8 Func_AD_K_B_Calibrate(u8 mode,u8 type,u8 wave,u8 std,u8 out_mode);//AD 8 ͨ�� K ,B У׼���� - 86
extern void Func_Plate_Run_Step(u8 mode,u8 plus);//����ø�����г� -87
extern u8 Func_Device_Self_Test(void);//����Լ�  -88
extern u8 Func_Std_Target_Test(u8 mode,u8 index);//  -89
extern u8 Func_Flash_Write_Read_Test(u8 len,u8 wave);//  -90


extern u8 Func_CRC32_Cmd_Check(u8* src, u8 len);//��֤У����
extern float Func_Get_Temperature(u8 index);//��ȡ�¶�

extern u8 Func_Temperature_Hot(u16 Target);//��������
extern u8 Func_Temperature_Cold(u16 Target);//��������
extern u8 Func_Temperature_Idle(u16 Target);//��������

#endif

