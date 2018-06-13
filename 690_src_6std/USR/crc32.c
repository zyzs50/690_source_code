#include "crc32.h"

uint32_t crc32_table[256]={0};  
uint32_t crc = 0xFFFFFFFF;  

/****************************************************************************
* 名    称：void make_crc32_table(void)
* 功    能：生成CRC32校验表
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void make_crc32_table(void)  
{  
    uint32_t c;  
    int i = 0;  
    int bit = 0;  
    for(i = 0; i < 256; i++)  
    {  
        c  = (uint32_t)i;        
        for(bit = 0; bit < 8; bit++)  
        {  
            if(c&1)  
            {  
                c = (c >> 1)^(0xEDB88320);  
            }  
            else  
            {  
                c =  c >> 1;  
            }         
        }  
        crc32_table[i] = c;  
// 				printf("crc32_table[%d] = %x\r\n",i,crc32_table[i]);
    }   
}  

/****************************************************************************
* 名    称：void make_crc(u8 *string, uint32_t size)
* 功    能：生成CRC32校验值
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
uint32_t make_crc(u8 *string, uint32_t size)  
{  
		uint32_t crc = 0xFFFFFFFF;  
    while(size--)  
        crc = (crc >> 8)^(crc32_table[(crc ^ *string++)&0xff]);
				crc = crc ^ 0xFFFFFFFF;
    return crc;  
}  

