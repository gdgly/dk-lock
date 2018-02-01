/*************************************************
  Copyright (C), 1988-1999,  Tech. Co., Ltd.
  File name:      
  Author:       Version:        Date:
  Description:   
  Others:         
  Function List:  
    1. ....
  History:         
                   
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _COMMON_H
#define _COMMON_H
#include <stm32f10x.h>


#define F(x, y, z)	(((x) & (y)) | ((~x) & (z)))
#define	G(x, y, z)	(((x) & (z)) | ((y) & (~z)))
#define H(x, y, z)	((x) ^ (y) ^ (z))
#define I(x, y, z)	((y) ^ ((x) | (~z)))



void delay(uint32_t conut);
void system_reset(void);
uint8_t *str_picked(uint8_t *src_str, uint8_t *sub_str1, uint8_t *sub_str2, uint8_t *dst_str);
uint8_t *mem_picked(uint8_t *src_str, uint8_t *sub_str, uint16_t len, uint8_t *dst_str);
char *itoa(int value, char *string, int radix);
char *mem_str(char *mem, int mem_len, char *sub_str);

#endif


