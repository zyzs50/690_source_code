#ifndef __FLASH_H__
#define __FLASH_H__
#include "pbdata.h"

//STM32F103RC FLASH 为 256KB
//分成 128个扇区， 每个扇区 2KB
//编号从 0 到 127

//256KB = 240KB + 16KB
//从0x8000000 开始 的240KB 用于存放用户代码 ：占 120 个扇区
//从0x803C000 开始 的16KB  用于存放用户数据 ：占 8   个扇区

//数据扇区划分：
//120:保存8个波长的校正因子： K ,B 
//121:保存8个波长的 T 
//122:保存8个波长的靶值




#define STM32_FLASH_BASE 0x08000000 	      //STM32 FLASH的起始地址

#define FLASH_SAVE_ADDR  0X0803C000 				//设置FLASH 保存地址(必须为偶数)
#define STM_SECTOR_SIZE	 2048               //扇区大小
#define STM32_FLASH_SIZE 256 	 		          //所选STM32的FLASH容量大小(单位为K)

extern u8 Flash_Write_K_B_T(u8 start_sector);
extern u8 Flash_Read_K_B_T(u8 start_sector);
extern u8 Flash_Write_Std_Target(u8 start_sector);
extern u8 Flash_Read_Std_Target(u8 start_sector);

extern u8 Flash_Write_Read_Test(u8 start_sector);

#endif

