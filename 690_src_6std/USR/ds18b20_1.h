#ifndef __DS18B20_H__
#define __DS18B20_H__
#include "pbdata.h"



// #define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
// #define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
// #define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
// #define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    
// #define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 

// #define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
// #define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

// #define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
// #define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}
// #define	DS18B20_DQ_OUT PGout(11) //���ݶ˿�	PA0 
// #define	DS18B20_DQ_IN  PGin(11)  //���ݶ˿�	PA0 






#define IO_DS18B20_1 GPIO_Pin_9
#define IO_DS18B20_2 GPIO_Pin_0

#define GPIO_DS18B20_1 GPIOB
#define GPIO_DS18B20_2 GPIOD

#define	DS18B20_1_DQ_IN  GPIO_ReadInputDataBit(GPIO_DS18B20_1,IO_DS18B20_1)
#define	DS18B20_2_DQ_IN  GPIO_ReadInputDataBit(GPIO_DS18B20_2,IO_DS18B20_2)

#define DS18B20_1_DQ_High GPIO_SetBits(GPIO_DS18B20_1,IO_DS18B20_1)
#define DS18B20_1_DQ_Low GPIO_ResetBits(GPIO_DS18B20_1,IO_DS18B20_1)

#define DS18B20_2_DQ_High GPIO_SetBits(GPIO_DS18B20_2,IO_DS18B20_2)
#define DS18B20_2_DQ_Low GPIO_ResetBits(GPIO_DS18B20_2,IO_DS18B20_2)


extern u8 G_Ds18b20_count;//�������ϵ�DS18B20��Ŀ
extern u8 G_DS18B20_ID[4][8];
extern u8 temp_dot_left;//�¶���������
extern u8 temp_dot_right;//�¶�С������


extern u8    DS18B20_Read_Byte(u8 channel);
extern void  DS18B20_Write_Byte(u8 dat,u8 channel);
extern void  DS18B20_Reset(u8 channel);//��λ
extern u16   DS18B20_Get_Temp(u8 channel);//���¶�
extern u8    DS18B20_Check(u8 channel);//DS18B20��ѯ�����Ƿ���Ӧ
extern u8    DS18B20_Init(u8 channel);//DS18B20��λ
//extern void  DS18B20_Read_Rom(u8 *str,u8 len);//��64λID
//extern u8    DS18B20_Match_Rom(u8 *rom);//ƥ��ROM
//extern u16   DS18B20_Get_wd(void);
//extern u8    DS18B20_Search_Rom_ID(void);
extern u8    DS18B20_Configuration(u8 channel);
extern float   Temperature_convert(u16 temp);




#endif

