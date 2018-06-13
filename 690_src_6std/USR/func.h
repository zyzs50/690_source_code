#ifndef __FUNC_H__
#define __FUNC_H__
#include "pbdata.h"

extern u8 Func_Front_Connect_Test(void);//联机测试 -03
extern u8 Func_Front_Self_Test(void);//前端板自检 -04
extern u8 Func_Sample_Test_Prepare(void);//准备样品测试 -06
extern u8 Func_Sample_Test_Start(void);//开始样本测试 -07
extern u8 Func_Sample_Test_Stop(void);//退出样本测试 -08
extern u8 Func_Com_On(void);//串口开启 - 09
extern u8 Func_Com_Off(void);//串口关闭 - 0A
extern u8 Func_USB_On(void);//USB开启 - 0B
extern u8 Func_USB_Off(void);//USB关闭 - 0C
extern u8 Func_Light_On(void);//光源关闭 - 0D
extern u8 Func_Light_Off(u8 fre);//光源关闭 - 0E
extern u8 Func_Temperature_Disp(void);//温育显示 - 13
extern u8 Func_Temperature_Heat(u16 Target);
extern u8 Func_Temperature_Off(void);//温育关闭 - 14
extern u8 Func_Peltier_Board_Heat(u16 Target);//制热 - 15
extern u8 Func_Peltier_Board_Cold(u16 Target);//制冷 - 16
extern u8 Func_Plate_Motor_Reset(u8 speed);//酶标板复位 - 17
extern u8 Func_Plate_Motor_Run(uint16_t speed,uint16_t step);//酶标板走板 - 18
extern u8 Func_Plate_Motor_Position(u8 speed,u8 index);//酶标板定位 - 19
extern u8 Func_Plate_Motor_Shake(u8 speed,u8 time);//同步带电机震动 -	1A
extern u8 Func_ABS_Test(void);//ABS测值 - 1B
extern u8 Func_ABS_Stop(void);//ABS停止 - 1C
extern u8 Func_LM_AD_Test(void);//光强停止 - 1D
extern u8 Func_LM_AD_Stop(void);//光源关闭 - 1E
extern u8 Func_Filter_Motor_Reset(u8 dir,u8 speed);//滤光轮复位 - 23
extern u8 Func_Filter_Motor_Position(u8 dir, u8 speed, u8 index);//滤光轮定位 - 24



extern u8 Func_Hardware_Test(void);//硬件测试-返回主板时钟  - 80
extern u8 Func_CRC32_Test(void);//crc32校验值 - 81
extern u8 Func_Beep_Test(u8 delay);//蜂鸣器验证 - 82
extern u8 Func_AD_Test(u8 mode,u8 wave,u8 channel,u8 filter_enable,u8 out_mode);//AD通道测试 - 83
extern u8 Func_Plate_Motor_Position_Loop(u8 mode,u8 speed);//酶标板循环定位测试 - 84
extern u8 Func_Wave_Sample(u8 wave_main,u8 wave_second,u8 col,u8 mode,u8 out_mode);//双波长测样 - 85
extern u8 Func_AD_K_B_Calibrate(u8 mode,u8 type,u8 wave,u8 std,u8 out_mode);//AD 8 通道 K ,B 校准测试 - 86
extern void Func_Plate_Run_Step(u8 mode,u8 plus);//测试酶标板的行程 -87
extern u8 Func_Device_Self_Test(void);//裸机自检  -88
extern u8 Func_Std_Target_Test(u8 mode,u8 index);//  -89
extern u8 Func_Flash_Write_Read_Test(u8 len,u8 wave);//  -90


extern u8 Func_CRC32_Cmd_Check(u8* src, u8 len);//验证校验码
extern float Func_Get_Temperature(u8 index);//读取温度

extern u8 Func_Temperature_Hot(u16 Target);//温育加热
extern u8 Func_Temperature_Cold(u16 Target);//温育制冷
extern u8 Func_Temperature_Idle(u16 Target);//温育待机

#endif

