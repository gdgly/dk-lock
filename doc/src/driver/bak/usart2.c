#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"

#include "misc.h"
#include "usart2.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "delay.h"
//#include "utils.h"

extern u16 USART2_RX_STA=0;   						//��������״̬

volatile unsigned char  gprs_ready_flag = 0;
volatile unsigned char  gprs_ready_count = 0;

unsigned char  usart2_rcv_buf[MAX_RCV_LEN];
volatile unsigned int   usart2_rcv_len = 0;


void USART2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* config USART2 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* USART2 GPIO config */
	/* Set PA2 PA3 as UART1 */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART2 mode config */
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);

    //Enable usart2 receive interrupt
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	  TIM3_Init(999,7199);		//10ms�ж�
	  usart2_rcv_len=0;		//����
	  TIM3_Set(0);			//�رն�ʱ��4
}

/*
*  @brief USART2?????????
*/
//void USART2_Clear(void)
//{
////	  memset(usart2_rcv_buf, 0, strlen(usart2_rcv_buf));
//    usart2_rcv_len = 0;
//}

///*
//*  @brief USART2????api
//*/
//void USART2_Write(USART_TypeDef* USARTx, uint8_t *Data, uint8_t len)
//{
//    uint8_t i;
//    USART_ClearFlag(USARTx, USART_FLAG_TC);
//    for(i = 0; i < len; i++)
//    {
//        USART_SendData(USARTx, *Data++);
//        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
//    }
//}



//void SendCmd(char* cmd, char* cmd1,char* result, char* result, int timeOut)
//{
//	  USART2_Clear();
//    while(1)
//    { 
//        USART2_Write(USART2, cmd, strlen(cmd));
//        delay_ms(timeOut);
//        printf("%s %d cmd:%s,rsp:%s\n", __func__, __LINE__, cmd, usart2_rcv_buf);
//        if((NULL != strstr(usart2_rcv_buf, result)))	//??????????
//        {
//					  usart2_rcv_len=0;
//					  myfree(usart2_rcv_buf);
//					  myfree(cmd);
//            break;
//        }
//        else
//        {
//            delay_ms(200);
//        }
//    }
//}


#if 1
/*
 *  @brief ??USART2????????
 */
uint32_t USART2_GetRcvNum(void)
{
	static uint32_t len = 0;
	uint32_t result = 0;
	
	if(usart2_rcv_len == 0)
	{
		len = 0;
		result = 0;
	}
	else if(len != usart2_rcv_len)
	{
		result = usart2_rcv_len - len;	//?????
		len = usart2_rcv_len;			//?????
	}
	
    return result;
}


void  USART2_GetRcvData(uint8_t *buf, uint32_t rcv_len)
{
    if(buf)
    {
        memcpy(buf, usart2_rcv_buf, rcv_len);
    }
    //USART2_Clear();
}
#endif


/**
  * @brief  This function handles usart2 global interrupt request.
  * @param  None
  * @retval : None
  */
void USART12_IRQHandler(void)
{
    unsigned int data;
#if 1
	/*
    if(USART2->SR & 0x0F)
    {
        // See if we have some kind of error
        // Clear interrupt (do nothing about it!)
       // data = USART2->DR;
			USART2->SR=0x00c0;		
    }
//    if(USART2->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
//    {
        
        data = USART2->DR;
       usart2_rcv_buf[usart2_rcv_len++] = data;
			
		USART2->SR=0x00c0;

		if(usart2_rcv_len >= (MAX_RCV_LEN - 1))
			  usart2_rcv_len = 0;
//        //usart1_rcv_buf[usart1_rcv_len++]=data;
//        //usart1_putrxchar(data);       //Insert received character into buffer
//    }

//    else
//    {
//        USART2->SR=0x00c0;
//			  USART1->DR='2';
//    }
	*/
	
//	 if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����  //Receive Data Reg Full Flag
//    {
			data=USART_ReceiveData(USART2);
//			usart2_rcv_buf[usart2_rcv_len++]=data;
//		  if(usart2_rcv_len >= (MAX_RCV_LEN - 1))
//			  usart2_rcv_len = 0;
			
			
			if(usart2_rcv_len<MAX_RCV_LEN)
			{
			usart2_rcv_buf[usart2_rcv_len++]=data;
				//USART1->DR=usart2_rcv_buf[usart2_rcv_len++];
//		  if(usart2_rcv_len >= (MAX_RCV_LEN - 1))
//			  usart2_rcv_len = 0;
//				TIM_SetCounter(TIM3,0);
//				if(usart2_rcv_len==0)TIM3_Set(1);
//				usart2_rcv_buf[usart2_rcv_len++]=data;
			}
//			else
//			{
//				usart2_rcv_len=0;
//			}
		//}
#endif
}


//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM3_Set(u8 sta)
{
	if(sta)
	{
       
		TIM_SetCounter(TIM3,0);//���������
		TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx	
	}else TIM_Cmd(TIM3, DISABLE);//�رն�ʱ��4	   
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM3_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��//TIM4ʱ��ʹ��    
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}






