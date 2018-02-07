#ifndef USART2_H_H
#define USART2_H_H

#include <stm32f10x.h>
#include <stdint.h>
#include "stm32f10x_tim.h"
#define MAX_RCV_LEN  1024
extern u16 USART2_RX_STA;   						//接收数据状态
extern void USART2_init(u32 bound);
extern void USART2_Write(USART_TypeDef* USARTx, uint8_t *Data,uint8_t len);
extern void USART2_Clear(void);
extern volatile unsigned char  gprs_ready_flag;
extern volatile unsigned char  gprs_ready_count;

extern unsigned char  usart2_rcv_buf[MAX_RCV_LEN];
extern volatile unsigned int   usart2_rcv_len;
void SendCmd(char* cmd, char* result, int timeOut);
//按时间进行
void TIM3_Set(u8 sta);
void TIM3_Init(u16 arr,u16 psc);
#endif













