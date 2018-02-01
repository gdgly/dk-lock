#include "key.h"
#include "delay.h"
//#include "usart.h"
#include "led.h"

 	    
//按键初始化函数 
//PA0.15和PC5 设置成输入
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTC时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	//锁是不是已经到位
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;//PA4，PA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA15
	//手动输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//PC6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC5
 
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
	
} 
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
//KEY0_PRES，KEY0按下
//KEY1_PRES，KEY1按下
//WKUP_PRES，WK_UP按下 
//注意此函数有响应优先级,KEY0>KEY1>WK_UP!!
u8 Hand_Flag=0;

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(HAND==0||LOCK_OFF==0||LOCK_ON==0))
	{
		//printf("123156465\r\n");
		delay_ms(10);//去抖动 
		key_up=0;
		if(HAND==0)return HAND_1;
		else if(LOCK_OFF==0)return LOCK_OFF_4;
		else if(LOCK_ON==0)return LOCK_ON_2; 
	}else if(HAND==1&&LOCK_OFF==1&&LOCK_ON==1)key_up=1; 	     
	return 0;// 无按键按下
}

 void Hand_Lock(void)
{
	if(HAND==0&&LOCK_OFF==0)//关锁/顺时针
	{
		Hand_Flag=1;
		Park_Lock_Close();
	}
	if(LOCK_ON==0&&Hand_Flag==1)
	{
		Park_Lock_stop();
		Hand_Flag=0;
	}
}

