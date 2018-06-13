#ifndef __HEAT_H__
#define __HEAT_H__
#include "pbdata.h"

#define HEAT_01_High GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define HEAT_01_Low  GPIO_ResetBits(GPIOB,GPIO_Pin_5)

#define HEAT_02_High GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define HEAT_02_Low  GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define HEAT_03_High GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define HEAT_03_Low  GPIO_ResetBits(GPIOB,GPIO_Pin_7)


#define HEAT1   1
#define HEAT2   2
#define HEAT3   3
#define HOTSTATE  1
#define IDLESTATE  2
#define COLDSTATE  3



extern void Heat3_Init(void);
extern void Peltier_Init(u8 state);
extern void Heat1_Control(uint16_t Target_Tem,u8 State);
extern void Heat2_Control(uint16_t Target_Tem,u8 State);
extern void Heat3_Control(uint16_t Target_Tem,u8 State);

#endif
