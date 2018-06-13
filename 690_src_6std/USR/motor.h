#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "pbdata.h"

extern int8_t step_index;//滤光轮电机4项8拍的索引
extern int8_t plate_motor_stop_flag;//同步带电机停止标志
extern int8_t filter_motor_stop_flag;//滤光轮电机停止标志

//同步带电机函数声明
extern uint8_t Plate_Motor_Run(uint8_t dir,uint8_t speed,uint16_t pluse);//同步带电机指定方向运行脉冲数
extern uint8_t Plate_Motor_Reset(uint8_t speed);//同步带电机复位
extern uint8_t Plate_Motor_Shake(uint8_t speed,uint8_t time);//同步带电机振动
extern uint8_t Plate_Motor_Position(uint8_t speed,uint8_t index);//同步带电机定位


//滤光轮电机函数声明
extern void Filter_Motor_Run(uint8_t dir);//滤光轮电机运行1步
extern uint8_t Filter_Motor_Location(uint8_t dir,uint8_t position,uint8_t speed);//滤光轮定位
extern uint8_t Filter_Motor_Reset(uint8_t dir,uint8_t speed);//滤光轮复位

//
extern uint8_t Filter_Plate_Motor_Loop(u8 mode,u8 speed);//同步带电机和滤光轮电机 轮询定位

#endif

