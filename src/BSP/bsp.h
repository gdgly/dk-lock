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
#ifndef _BSP_H
#define _BSP_H
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_iwdg.h>
#include <misc.h>



#define ENABLE						1
#define DISABLE						0


#define SCL_HIGH()				GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define SCL_LOW()				GPIO_ResetBits(GPIOB, GPIO_Pin_0)

#define SDA_HIGH()				GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define SDA_LOW()				GPIO_ResetBits(GPIOB, GPIO_Pin_1)

#define SDA_READ()				GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)

#define BUTTON1_READ()			GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)		//����


#define LOCK_ON_READ()			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)
#define LOCK_OFF_READ()			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)



#define HAND()  				GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)//�ֶ�����
//#define ALARM_LOCK_ON()  		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//��������
//#define ALARM_LOCK_OFF()   		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//��������



#define MOTOA_HIGH() 			GPIO_SetBits(GPIOC, GPIO_Pin_0)	
#define MOTOA_LOW() 			GPIO_ResetBits(GPIOC, GPIO_Pin_0)

#define MOTOB_HIGH() 			GPIO_SetBits(GPIOC, GPIO_Pin_1)	
#define MOTOB_LOW() 			GPIO_ResetBits(GPIOC, GPIO_Pin_1)


#define BEEP_ON() 			GPIO_SetBits(GPIOC, GPIO_Pin_3)	
#define BEEP_OFF() 				GPIO_ResetBits(GPIOC, GPIO_Pin_3)

#define GPS_POW_HIGH()			GPIO_SetBits(GPIOC, GPIO_Pin_4)	
#define GPS_POW_LOW()			GPIO_ResetBits(GPIOC, GPIO_Pin_4)	


//#define NVIC_VECTOR_ADDR			0x1C00

//#define NVIC_VECTOR_ADDR			0x0000

void iwatchdog_clear(void);
void gprs_config(void);
void bsp_init(void);


void lock_on(void);
void lock_off(void);
void lock_close(void);


u8 lock_on_status_get(void);
u8 lock_off_status_get(void);

#endif



