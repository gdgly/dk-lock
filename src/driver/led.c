#include "led.h"

 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_BEEP_MOTO_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
 GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1);						 //PA.8 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;	    		 //LED1-->PD.2 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_ResetBits(GPIOC,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4); 						 //PD.2 输出高 
}
 void Park_Lock_Open(void)//开锁
 {
	 MOTOR_A=0;
	 MOTOR_B=1;
 }
 void Park_Lock_Close(void)//关锁
 {
	 MOTOR_A=1;
	 MOTOR_B=0;
 }

  void Park_Lock_stop(void)//停止开关锁
 {
	 MOTOR_A=0;
	 MOTOR_B=0;
 }

	







 