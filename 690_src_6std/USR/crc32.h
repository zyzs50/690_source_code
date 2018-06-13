#ifndef __CRC32_H__
#define __CRC32_H__

#include "pbdata.h"

extern uint32_t crc32_table[256];  
extern uint32_t crc;  

extern void make_crc32_table(void);  
extern uint32_t make_crc(u8 *string, uint32_t size);  




#endif

