#ifndef __KEY_H
#define __KEY_H	 
//#include "sys.h"

#include "bsp.h"


//#define KEY0 PCin(5)   	
//#define KEY1 PAin(15)	 
//#define WK_UP  PAin(0)	 
 

#define HAND  		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)//手动输入
#define LOCK_ON  	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//关锁报警
#define LOCK_OFF   	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//开锁报警
 

#define HAND_1	1		//KEY0  
#define LOCK_ON_2	2		//KEY1 
#define LOCK_OFF_4	4		//WK_UP  

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数		

//手动关锁
 void Hand_Lock(void);
#endif
