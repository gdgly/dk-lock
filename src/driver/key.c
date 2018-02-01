#include "key.h"
#include "delay.h"
//#include "usart.h"
#include "led.h"

 	    
//������ʼ������ 
//PA0.15��PC5 ���ó�����
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTCʱ��

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	//���ǲ����Ѿ���λ
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;//PA4��PA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA15
	//�ֶ�����
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//PC6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC5
 
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
	
} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES��KEY0����
//KEY1_PRES��KEY1����
//WKUP_PRES��WK_UP���� 
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 Hand_Flag=0;

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(HAND==0||LOCK_OFF==0||LOCK_ON==0))
	{
		//printf("123156465\r\n");
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(HAND==0)return HAND_1;
		else if(LOCK_OFF==0)return LOCK_OFF_4;
		else if(LOCK_ON==0)return LOCK_ON_2; 
	}else if(HAND==1&&LOCK_OFF==1&&LOCK_ON==1)key_up=1; 	     
	return 0;// �ް�������
}

 void Hand_Lock(void)
{
	if(HAND==0&&LOCK_OFF==0)//����/˳ʱ��
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

