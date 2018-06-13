#ifndef __FLASH_H__
#define __FLASH_H__
#include "pbdata.h"

//STM32F103RC FLASH Ϊ 256KB
//�ֳ� 128�������� ÿ������ 2KB
//��Ŵ� 0 �� 127

//256KB = 240KB + 16KB
//��0x8000000 ��ʼ ��240KB ���ڴ���û����� ��ռ 120 ������
//��0x803C000 ��ʼ ��16KB  ���ڴ���û����� ��ռ 8   ������

//�����������֣�
//120:����8��������У�����ӣ� K ,B 
//121:����8�������� T 
//122:����8�������İ�ֵ




#define STM32_FLASH_BASE 0x08000000 	      //STM32 FLASH����ʼ��ַ

#define FLASH_SAVE_ADDR  0X0803C000 				//����FLASH �����ַ(����Ϊż��)
#define STM_SECTOR_SIZE	 2048               //������С
#define STM32_FLASH_SIZE 256 	 		          //��ѡSTM32��FLASH������С(��λΪK)

extern u8 Flash_Write_K_B_T(u8 start_sector);
extern u8 Flash_Read_K_B_T(u8 start_sector);
extern u8 Flash_Write_Std_Target(u8 start_sector);
extern u8 Flash_Read_Std_Target(u8 start_sector);

extern u8 Flash_Write_Read_Test(u8 start_sector);

#endif

