#include "flash.h"
#include "stm32f10x_flash.h"
#include "calibrate.h"
#include "usb_config.h"



u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�, ���һ����������
u16 stm32_buf[1024];//�����2K�ֽ�, ���һ����������


/****************************************************************************
* ��    �ƣ�void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
* ��    �ܣ�FLASH д��
* ��ڲ���0��WriteAddr ������Ŀ�ĵ�ַ
* ��ڲ���1��pBuffer   ������Դ��ַ
* ��ڲ���2��NumToWrite�����ݴ�С
* ���ڲ�����
* ˵    ����
* ���÷���:
****************************************************************************/
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//��ַ����2.
	}  
} 

/****************************************************************************
* ��    �ƣ�u16 STMFLASH_ReadHalfWord(u32 faddr)
* ��    �ܣ�FLASH ����һ��2�ֽ�����
* ��ڲ���0��faddr �����ݵ�ַ
* ���ڲ�����
* ˵    ����
* ���÷���:
****************************************************************************/
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

/****************************************************************************
* ��    �ƣ�void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)
* ��    �ܣ�FLASH ����
* ��ڲ���0��ReadAddr ������Ŀ�ĵ�ַ
* ��ڲ���1��pBuffer   ������Դ��ַ
* ��ڲ���2��NumToRead�����ݴ�С
* ���ڲ�����
* ˵    ����
* ���÷���:
****************************************************************************/
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}


/****************************************************************************
* ��    �ƣ�void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
* ��    �ܣ�FLASH д��
* ��ڲ���0��WriteAddr ������Ŀ�ĵ�ַ
* ��ڲ���1��pBuffer   ������Դ��ַ
* ��ڲ���2��NumToWrite�����ݴ�С
* ���ڲ�����
* ˵    ����
* ���÷���:
****************************************************************************/
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)
	{
		secremain=NumToWrite;//�����ڸ�������Χ
	}
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,stm32_buf,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(stm32_buf[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				stm32_buf[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,stm32_buf,STM_SECTOR_SIZE/2);//д����������  
		}
		else
		{
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		}
		
		if(NumToWrite==secremain)
		{
			break;//д�������
		}
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		  pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		  NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))
			{
				secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			}
			else
			{
				secremain=NumToWrite;//��һ����������д����
			}				
		}	 
	};	
	FLASH_Lock();//����
}




/****************************************************************************
* ��    �ƣ�u8 Flash_Write_K_B_T(u8 start_sector)
* ��    �ܣ�FLASH д�� У������
* ��ڲ���0��start_sector ��Ҫд����������
* ���ڲ�����
* ˵    ������������ռ8���� ��120��ʼ   127������  K,B,T����3������:120,121,122
* ���÷���:
****************************************************************************/
u8 Flash_Write_K_B_T(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[8];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	//ע�������K������ 120������
	//ע�������B������ 121������
	//ע�������T������ 122������
	
	//��Ҫд���K,B�����120������
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
	//�� K д��flash
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
	//�� B д��flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*(start_sector+1);
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,k);

	
	k = 0;
	//�� T ����� 121 ������
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
	
	//�� T д��flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*(start_sector+2);
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,k);
	return 0;
}


/****************************************************************************
* ��    �ƣ�u8 Flash_Read_K_B_T(u8 start_sector)
* ��    �ܣ�FLASH ���� У������
* ��ڲ���0��start_sector ��Ҫ������������
* ���ڲ�����
* ˵    ������������ռ8���� ��120��ʼ   127������  K,B,T����3������:120,121,122
* ���÷���:
****************************************************************************/
u8 Flash_Read_K_B_T(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[8];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	//����K
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
	//����B
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
	//����T
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
* ��    �ƣ�u8 Flash_Write_Std_Target(u8 start_sector)
* ��    �ܣ�FLASH д�� ��׼���ʵİ�ֵ��
* ��ڲ���0��start_sector ��Ҫд����������
* ���ڲ�����
* ˵    ������������ռ8���� ��120��ʼ   127������  ��ֵ�����1������: 123
* ���÷���:
****************************************************************************/
u8 Flash_Write_Std_Target(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[3];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	//��Ҫд��ı�׼���ʵİ�ֵ������ ������������
	
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
	
	//д��flash
  dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,k);

	return 0;
}


/****************************************************************************
* ��    �ƣ�u8 Flash_Read_Std_Target(u8 start_sector)
* ��    �ܣ�FLASH ���� ��׼���ʵİ�ֵ��
* ��ڲ���0��start_sector ��Ҫд����������
* ���ڲ�����
* ˵    ������������ռ8���� ��120��ʼ   127������  ��ֵ�����1������: 123
* ���÷���:
****************************************************************************/
u8 Flash_Read_Std_Target(u8 start_sector)
{
	u8  i=0;
	u8  j=0;
	u16 k=0;
	u16 *addr_array[3];
	u16 *addr=0;
	u32 dst_addr = 0;
	
	
	//����flash
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
* ��    �ƣ�u8 Flash_Write_Read_Test(u8 start_sector)
* ��    �ܣ�FLASH д�� ���� ����
* ��ڲ���0��start_sector ��Ҫд����������
* ���ڲ�����
* ˵    ������������ռ8���� ��120��ʼ   127������  ��ֵ�����1������: 123
* ���÷���:
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

	//д��flash
	dst_addr = STM32_FLASH_BASE + STM_SECTOR_SIZE*start_sector;
	STMFLASH_Write(dst_addr,(u16*)STMFLASH_BUF,1024);

	for(i=0;i<1024;i++)
	{
		STMFLASH_BUF[i]=0;
	}
	//����flash
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
