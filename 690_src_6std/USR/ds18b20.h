#ifndef __DS18B20_H__
#define __DS18B20_H__
#include "pbdata.h"




#define GPIO_DS18B20_1 GPIOB
#define IO_DS18B20_1   GPIO_Pin_9
#define	DS18B20_1_DQ_IN  GPIO_ReadInputDataBit(GPIO_DS18B20_1,IO_DS18B20_1)
#define DS18B20_1_DQ_High GPIO_SetBits(GPIO_DS18B20_1,IO_DS18B20_1)
#define DS18B20_1_DQ_Low GPIO_ResetBits(GPIO_DS18B20_1,IO_DS18B20_1)




#define GPIO_DS18B20_2 GPIOD
#define IO_DS18B20_2 GPIO_Pin_0
#define	DS18B20_2_DQ_IN  GPIO_ReadInputDataBit(GPIO_DS18B20_2,IO_DS18B20_2)
#define DS18B20_2_DQ_High GPIO_SetBits(GPIO_DS18B20_2,IO_DS18B20_2)
#define DS18B20_2_DQ_Low GPIO_ResetBits(GPIO_DS18B20_2,IO_DS18B20_2)





extern u8    DS18B20_Read_Byte(u8 channel);
extern void  DS18B20_Write_Byte(u8 dat,u8 channel);
extern void  DS18B20_Reset(u8 channel);//复位
extern u8    DS18B20_Check(u8 channel);//DS18B20查询器件是否响应
extern u8    DS18B20_Init(u8 channel);//DS18B20复位
extern u16   DS18B20_Get_wd(u8 channel);
extern u8    DS18B20_Configuration(u8 channel);
extern u16   Temperature_convert(u16 temp);




#endif

