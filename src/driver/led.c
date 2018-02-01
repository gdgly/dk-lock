#include "led.h"

 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_BEEP_MOTO_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
 GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1);						 //PA.8 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;	    		 //LED1-->PD.2 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_ResetBits(GPIOC,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4); 						 //PD.2 ����� 
}
 void Park_Lock_Open(void)//����
 {
	 MOTOR_A=0;
	 MOTOR_B=1;
 }
 void Park_Lock_Close(void)//����
 {
	 MOTOR_A=1;
	 MOTOR_B=0;
 }

  void Park_Lock_stop(void)//ֹͣ������
 {
	 MOTOR_A=0;
	 MOTOR_B=0;
 }

	







 