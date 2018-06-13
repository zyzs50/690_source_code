#include "flash.h"
#include "stm32f10x_flash.h"
#include "calibrate.h"
#include "usb_config.h"



u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节, 存放一扇区的内容
u16 stm32_buf[1024];//最多是2K字节, 存放一扇区的内容


/****************************************************************************
* 名    称：void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
* 功    能：FLASH 写入
* 入口参数0：WriteAddr ：数据目的地址
* 入口参数1：pBuffer   ：数据源地址
* 入口参数2：NumToWrite：数据大小
* 出口参数：
* 说    明：
* 调用方法:
****************************************************************************/
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//地址增加2.
	}  
} 

/****************************************************************************
* 名    称：u16 STMFLASH_ReadHalfWord(u32 faddr)
* 功    能：FLASH 读入一个2字节数据
* 入口参数0：faddr ：数据地址
* 出口参数：
* 说    明：
* 调用方法:
****************************************************************************/
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

/****************************************************************************
* 名    称：void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)
* 功    能：FLASH 读入
* 入口参数0：ReadAddr ：数据目的地址
* 入口参数1：pBuffer   ：数据源地址
* 入口参数2：NumToRead：数据大小
* 出口参数：
* 说    明：
* 调用方法:
****************************************************************************/
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}


/****************************************************************************
* 名    称：void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
* 功    能：FLASH 写入
* 入口参数0：WriteAddr ：数据目的地址
* 入口参数1：pBuffer   ：数据源地址
* 入口参数2：NumToWrite：数据大小
* 出口参数：
* 说    明：
* 调用方法:
****************************************************************************/
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)
	{
		secremain=NumToWrite;//不大于该扇区范围
	}
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,stm32_buf,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(stm32_buf[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				stm32_buf[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,stm32_buf,STM_SECTOR_SIZE/2);//写入整个扇区  
		}
		else
		{
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		}
		
		if(NumToWrite==secremain)
		{
			break;//写入结束了
		}
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		  pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		  NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))
			{
				secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			}
			else
			{
				secremain=NumToWrite;//下一个扇区可以写完了
			}				
		}	 
	};	
	FLASH_Lock();//上锁
}




/****************************************************************************
* 名    称：u8 Flash_Write_K_B_T(u8 start_sector)
* 功    能：FLASH 写入 校正因子
* 入口参数0：start_sector ：要写入的扇区编号
* 出口参数：
* 说    明：数据扇区占8个， 从120开始   127结束，  K,B,T分配3个扇区:120,121,122
* 调用方法:
****************************************************************************/
u8 Flash_Write_K_B_T(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[8];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	//注意这里的K分配在 120号扇区
	//注意这里的B分配在 121号扇区
	//注意这里的T分配在 122号扇区
	
	//将要写入的K,B存放在120号扇区
	addr_array[0] = (u16*)&Slope_405nm[0][0];
	addr_array[1] = (u16*)&Slope_450nm[0][0];
	addr_array[2] = (u16*)&Slope_492nm[0][0];
	addr_array[3] = (u16*)&Slope_630nm[0][0];
	addr_array[4] = (u16*)&Slope_Ext01nm[0][0];
	addr_array[5] = (u16*)&Slope_Ext02nm[0][0];
	addr_array[6] = (u16*)&Slope_Ext03nm[0][0];
	addr_array[7] = (u16*)&Slope_Ext04nm[0][0];
	
	for(i=0;i<8;i++)
	{
		addr = addr_array[i];
		for(j=0;j<96;j++)
		{
			STMFLASH_BUF[k] = *addr;
			addr += 1;
			k += 1; 
		}
	}
	//将 K 写入flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,k);
	
	
	k = 0;
	addr_array[0] = (u16*)&Intercept_405nm[0][0];
	addr_array[1] = (u16*)&Intercept_450nm[0][0];
	addr_array[2] = (u16*)&Intercept_492nm[0][0];
	addr_array[3] = (u16*)&Intercept_630nm[0][0];
	addr_array[4] = (u16*)&Intercept_Ext01nm[0][0];
	addr_array[5] = (u16*)&Intercept_Ext02nm[0][0];
	addr_array[6] = (u16*)&Intercept_Ext03nm[0][0];
	addr_array[7] = (u16*)&Intercept_Ext04nm[0][0];
	
	
	for(i=0;i<8;i++)
	{
		addr = addr_array[i];
		for(j=0;j<96;j++)
		{
			STMFLASH_BUF[k] = *addr;
			addr += 1;
			k += 1; 
		}
	}
	//将 B 写入flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*(start_sector+1);
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,k);

	
	k = 0;
	//将 T 存放在 121 号扇区
	addr_array[0] = (u16*)&TestValue_405nm[0][0];
	addr_array[1] = (u16*)&TestValue_450nm[0][0];
	addr_array[2] = (u16*)&TestValue_492nm[0][0];
	addr_array[3] = (u16*)&TestValue_630nm[0][0];
	addr_array[4] = (u16*)&TestValue_Ext01nm[0][0];
	addr_array[5] = (u16*)&TestValue_Ext02nm[0][0];
	addr_array[6] = (u16*)&TestValue_Ext03nm[0][0];
	addr_array[7] = (u16*)&TestValue_Ext04nm[0][0];
	
	for(i=0;i<8;i++)
	{
		addr = addr_array[i];
		for(j=0;j<80;j++)
		{
			STMFLASH_BUF[k] = *addr;
			addr += 1;
			k += 1; 
		}
	}
	
	//将 T 写入flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*(start_sector+2);
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,k);
	return 0;
}


/****************************************************************************
* 名    称：u8 Flash_Read_K_B_T(u8 start_sector)
* 功    能：FLASH 读出 校正因子
* 入口参数0：start_sector ：要读入的扇区编号
* 出口参数：
* 说    明：数据扇区占8个， 从120开始   127结束，  K,B,T分配3个扇区:120,121,122
* 调用方法:
****************************************************************************/
u8 Flash_Read_K_B_T(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[8];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	//读出K
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Read(dst_addr,(u16*)STMFLASH_BUF,768);
	
	addr_array[0] = (u16*)&Slope_405nm[0][0];
	addr_array[1] = (u16*)&Slope_450nm[0][0];
	addr_array[2] = (u16*)&Slope_492nm[0][0];
	addr_array[3] = (u16*)&Slope_630nm[0][0];
	addr_array[4] = (u16*)&Slope_Ext01nm[0][0];
	addr_array[5] = (u16*)&Slope_Ext02nm[0][0];
	addr_array[6] = (u16*)&Slope_Ext03nm[0][0];
	addr_array[7] = (u16*)&Slope_Ext04nm[0][0];
	
	for(i=0;i<8;i++)
	{
		addr = addr_array[i];
		for(j=0;j<96;j++)
		{
			*addr = STMFLASH_BUF[k];
			addr += 1;
			k += 1; 
		}
	}
	
	
	k=0;
	//读出B
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*(start_sector+1);
	STMFLASH_Read(dst_addr,(u16*)STMFLASH_BUF,768);
	
	addr_array[0] = (u16*)&Intercept_405nm[0][0];
	addr_array[1] = (u16*)&Intercept_450nm[0][0];
	addr_array[2] = (u16*)&Intercept_492nm[0][0];
	addr_array[3] = (u16*)&Intercept_630nm[0][0];
	addr_array[4] = (u16*)&Intercept_Ext01nm[0][0];
	addr_array[5] = (u16*)&Intercept_Ext02nm[0][0];
	addr_array[6] = (u16*)&Intercept_Ext03nm[0][0];
	addr_array[7] = (u16*)&Intercept_Ext04nm[0][0];
	
	for(i=0;i<8;i++)
	{
		addr = addr_array[i];
		for(j=0;j<96;j++)
		{
			*addr = STMFLASH_BUF[k];
			addr += 1;
			k += 1; 
		}
	}
	
	k=0;
	//读出T
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*(start_sector+2);
	STMFLASH_Read(dst_addr,(u16*)STMFLASH_BUF,640);
	
	addr_array[0] = (u16*)&TestValue_405nm[0][0];
	addr_array[1] = (u16*)&TestValue_450nm[0][0];
	addr_array[2] = (u16*)&TestValue_492nm[0][0];
	addr_array[3] = (u16*)&TestValue_630nm[0][0];
	addr_array[4] = (u16*)&TestValue_Ext01nm[0][0];
	addr_array[5] = (u16*)&TestValue_Ext02nm[0][0];
	addr_array[6] = (u16*)&TestValue_Ext03nm[0][0];
	addr_array[7] = (u16*)&TestValue_Ext04nm[0][0];
	
	for(i=0;i<8;i++)
	{
		addr = addr_array[i];
		for(j=0;j<80;j++)
		{
			*addr = STMFLASH_BUF[k];
			addr += 1;
			k += 1; 
		}
	}	
	
	return 0;
}


/****************************************************************************
* 名    称：u8 Flash_Write_Std_Target(u8 start_sector)
* 功    能：FLASH 写入 标准物质的靶值表
* 入口参数0：start_sector ：要写入的扇区编号
* 出口参数：
* 说    明：数据扇区占8个， 从120开始   127结束，  靶值表分配1个扇区: 123
* 调用方法:
****************************************************************************/
u8 Flash_Write_Std_Target(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[3];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	//将要写入的标准物质的靶值表存放在 扇区缓冲区中
	
	addr_array[0] = (u16*)&targetValueA[0];
	addr_array[1] = (u16*)&targetValueB[0];
	addr_array[2] = (u16*)&targetValueC[0];
	
	for(i=0;i<3;i++)
	{
		addr = addr_array[i];
		for(j=0;j<48;j++)
		{
			STMFLASH_BUF[k] = *addr;
			addr += 1;
			k += 1; 
		}
	}
	
	//写入flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,k);

	return 0;
}


/****************************************************************************
* 名    称：u8 Flash_Read_Std_Target(u8 start_sector)
* 功    能：FLASH 读出 标准物质的靶值表
* 入口参数0：start_sector ：要写入的扇区编号
* 出口参数：
* 说    明：数据扇区占8个， 从120开始   127结束，  靶值表分配1个扇区: 123
* 调用方法:
****************************************************************************/
u8 Flash_Read_Std_Target(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[3];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	
	//读出flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Read(dst_addr,(u16*)STMFLASH_BUF,144);
	
	addr_array[0] = (u16*)&targetValueA[0];
	addr_array[1] = (u16*)&targetValueB[0];
	addr_array[2] = (u16*)&targetValueC[0];
	
	for(i=0;i<3;i++)
	{
		addr = addr_array[i];
		for(j=0;j<48;j++)
		{
			*addr = STMFLASH_BUF[k];
			addr += 1;
			k += 1; 
		}
	}
	return 0;
}

/****************************************************************************
* 名    称：u8 Flash_Write_Read_Test(u8 start_sector)
* 功    能：FLASH 写入 读出 测试
* 入口参数0：start_sector ：要写入的扇区编号
* 出口参数：
* 说    明：数据扇区占8个， 从120开始   127结束，  靶值表分配1个扇区: 123
* 调用方法:
****************************************************************************/
u8 Flash_Write_Read_Test(u8 start_sector)
{
	u16 i =0;
	u32 dst_addr =0;
	
	for(i=0;i<1024;i++)
	{
		STMFLASH_BUF[i]=i;
	}
	
	printf("------------STMFLASH_BUF0------------------------\r\n");
	for(i=0;i<16;i++)
	{
		USB_TxWrite((u8*)STMFLASH_BUF+i*128, 128);
		delay_ms(10);
	}

	//写入flash
	dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,1024);

	for(i=0;i<1024;i++)
	{
		STMFLASH_BUF[i]=0;
	}
	//读出flash
	dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Read(dst_addr,(u16*)STMFLASH_BUF,1024);

	printf("------------STMFLASH_BUF1------------------------\r\n");
	for(i=0;i<16;i++)
	{
		USB_TxWrite((u8*)STMFLASH_BUF+i*128, 128);
		delay_ms(10);
	}
	
	return 0;
}
