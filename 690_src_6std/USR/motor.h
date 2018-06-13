#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "pbdata.h"

extern int8_t step_index;//�˹��ֵ��4��8�ĵ�����
extern int8_t plate_motor_stop_flag;//ͬ�������ֹͣ��־
extern int8_t filter_motor_stop_flag;//�˹��ֵ��ֹͣ��־

//ͬ���������������
extern uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse);//ͬ�������ָ����������������
extern uint8_t Plate_Motor_Reset(uint8_t speed);//ͬ���������λ
extern uint8_t Plate_Motor_Shake(uint8_t speed,uint8_t time);//ͬ���������
extern uint8_t Plate_Motor_Position(uint8_t speed,uint8_t index);//ͬ���������λ


//�˹��ֵ����������
extern void Filter_Motor_Run(uint8_t dir);//�˹��ֵ������1��
extern uint8_t Filter_Motor_Location(uint8_t dir,uint8_t position,uint8_t speed);//�˹��ֶ�λ
extern uint8_t Filter_Motor_Reset(uint8_t dir,uint8_t speed);//�˹��ָ�λ

//
extern uint8_t Filter_Plate_Motor_Loop(u8 mode,u8 speed);//ͬ����������˹��ֵ�� ��ѯ��λ

#endif

