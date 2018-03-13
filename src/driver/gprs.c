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

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include "gprs.h"
#include "bsp.h"
#include "usart.h"
#include "timer.h"
#include "transport.h"





extern usart_buff_t  *usart1_rx_buff;
extern usart_buff_t  *usart2_rx_buff;


//extern u8 usart2_buff[512];
//extern u16 usart2_cnt;

extern u8 usart2_rx_status;





uint8_t gprs_err_cnt = 0; 	//GPRS���������
uint8_t gprs_status = 0;	//GPRS��״̬

uint8_t gprs_rx_buf[512];
uint16_t gprs_rx_cnt = 0;

uint8_t gprs_send_at_flag = 0;	
uint8_t gprs_rx_flag = 0;



u8 PARK_LOCK_Buffer[17] = {0};
extern u8 lock_id[16];
u8 topic_id = 0;

/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void gprs_power_on(void)
{
//	OS_ERR err;
//	
//	GPIO_SetBits(GPIOC, GPIO_Pin_1);				//GPRS_PWR
//	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
////	OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);

//	
//	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
////	OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err);
//	GPIO_SetBits(GPIOA, GPIO_Pin_7);				//GPRS_RESET
////	OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err);

//	
//	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
////	OSTimeDlyHMSM(0,0,1,100,OS_OPT_TIME_HMSM_STRICT,&err);
	
}






/*
*********************************************************************************************************
*                                          gprs_check_cmd()
*
* Description : ����ַ������Ƿ�������ַ���.
*
* Argument(s) : p_str �� Ҫ�������ַ���
*
* Return(s)   : �������ַ�����λ��ָ��
*
* Caller(s)   : gprs_send_at()
*
* Note(s)     : 
*********************************************************************************************************
*/
u8 *gprs_check_cmd(u8 *src_str, u8 *p_str)
{
	char *str = NULL;
	
	str = strstr((const char*)src_str, (const char*)p_str);

	return (u8*)str;
}


/*
*********************************************************************************************************
*                                          gprs_send_at()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/



u8* gprs_send_at(u8 *cmd, u8 *ack, u16 waittime, u16 timeout)
{
	u8 res = 1;
	u8 buff[512];
	
	timer_is_timeout_1ms(timer_at, 0);	//��ʼ��ʱ��timer_at
	while (res)
	{	
		memset(usart2_rx_buff, 0, sizeof(usart_buff_t));
		
		usart2_rx_status = 0;
		USART_OUT(USART2, cmd);							
		timer_delay_1ms(waittime);				//ATָ����ʱ
	
		usart2_rx_status = 1;	//����δ���� ���ڽ�������
		USART_OUT(USART1, usart2_rx_buff->pdata);

		if (gprs_check_cmd(usart2_rx_buff->pdata, ack))	
		{
			res = 0;				//��⵽��ȷ��Ӧ������
			usart2_rx_status = 0;	//���ݴ����� ��ʼ��������
			
			memcpy(buff, usart2_rx_buff->pdata, 512);
			
			memset(usart2_rx_buff, 0, sizeof(usart_buff_t));	
			
			return buff;
		}
			
		if (timer_is_timeout_1ms(timer_at, timeout) == 0)	//��ʱ��timer_at����
		{
			res = 0;
			usart2_rx_status = 0;	//���ݴ����� ��ʼ��������
			return NULL;
		}
	}	
}



void gprs_send_data(u8 *data, u16 data_len, u16 waittime)
{
	u8 res = 1;
	u8 buff[512];	

	usart_send(USART2, data, data_len);	
	
	timer_delay_1ms(waittime);				//ATָ����ʱ

	USART_OUT(USART1, usart2_rx_buff->pdata);
		
}


void gprs_config(void)
{
	u8 *ret;
	u8 buff[512] = {0};
	char *str = 0;
	switch(gprs_status)
	{
		
		case 0:
			gprs_power_on();
			timer_delay_1ms(1000);
			gprs_status++;
		break;
		
		case 1:
			ret = gprs_send_at("AT\r\n", "OK", 800,10000);
			if (ret != NULL)
			{
				gprs_status++;
				gprs_err_cnt = 0;
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;
			
		case 2:
			ret = gprs_send_at("AT+STATUS\r\n", "OK", 800,10000);
			if (ret != NULL)
			{
				if(strstr((char*)ret, "MQTT READY") != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
					USART_OUT(USART1, "STATUS=%d\r\n", gprs_status);
				}
				else if(strstr((char*)ret, "MQTT CONNECT OK") != NULL)
				{
					gprs_status = 4;
					gprs_err_cnt = 0;
				}
				else if(strstr((char*)ret, "MQTT CLOSE") != NULL)
				{
					gprs_status = 0;
				}
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;	
		
		case 3:
			ret = gprs_send_at("AT+START\r\n", "OK", 800,5000);
			if (ret != NULL)
			{
				gprs_status++;
				gprs_err_cnt = 0;
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;	
		
			
		case 4:
			ret = gprs_send_at("AT+STATUS\r\n", "MQTT CONNECT OK", 800,5000);
			if (ret != NULL)
			{
				gprs_status++;
				gprs_err_cnt = 0;
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;		
		
		case 5:
			ret = gprs_send_at("AT+CLIENTID?\r\n", "OK", 50, 1000);
			if (ret != NULL)
			{
				USART_OUT(USART1, ret);
				str = strstr((const char*)ret, (const char*)"+CLIENTID: ");
				memcpy(PARK_LOCK_Buffer, str+12, 16);	//������id	
				USART_OUT(USART1, "lock_id=%s\r\n", PARK_LOCK_Buffer);
				gprs_status++;
				gprs_err_cnt = 0;
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;
			
		case 6:
			ret = gprs_send_at("AT+STATUS\r\n", "MQTT CONNECT OK", 800,5000);
			if (ret != NULL)
			{
				gprs_status++;
				gprs_err_cnt = 0;
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;
			
		case 7:
			sprintf((char*)buff, "%s%s%s", "AT+SUBSCRIBE=bell/", PARK_LOCK_Buffer, ",2\r\n");
			USART_OUT(USART1, "lock_id=%s\r\n", buff);
			ret = gprs_send_at(buff, "OK", 800, 2000);
			if (ret != NULL)
			{
				gprs_status++;
				gprs_err_cnt = 0;
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;
			
		case 8:
			sprintf((char*)buff, "%s%s%s", "AT+SUBSCRIBE=lock/", PARK_LOCK_Buffer, ",2\r\n");
			USART_OUT(USART1, "lock=%s\r\n", buff);
			ret = gprs_send_at(buff, "OK", 800, 2000);
			if (ret != NULL)
			{
				gprs_status++;
				gprs_err_cnt = 0;
			}
			else
			{
				gprs_err_cnt++;
				if (gprs_err_cnt > 5)
				{
					gprs_status = 0;
				}
			}
		break;
			
		case 9:
			gprs_status = 255;
		break;
			
		
		default:
		break;				
				
	}
	
	
	
	
}







/*
*********************************************************************************************************
*                                          gprs_init_task_fun()
*
* Description : GPRS��ʼ��������.
*
* Argument(s) : p_arg :������Ҫ���Ĳ���
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void gprs_init_task(void)
{

	int mqtt_con = 0;
	u8 *ret;
	static u8 gprs_init_flag = true;		//
		
	while(1)
	{
		switch(gprs_status)
		{
			case 0:
//				gprs_power_on();
//				OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
//				ret = gprs_send_at("AT+CFUN=1,1\r\n", "OK", 800,10000);
//				if(ret != NULL)
				{
					gprs_status = 1;
					gprs_err_cnt = 0;
				}
			break;
					
			case 1:
				ret = gprs_send_at("AT\r\n", "OK", 800,10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 2:
				ret = gprs_send_at("ATE1\r\n", "OK", 800,10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
				
			case 3:
				ret = gprs_send_at("AT+GSN\r\n", "OK", 800, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 4:			
				ret = gprs_send_at("AT+CPIN?\r\n", "OK", 800, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 5:
				ret = gprs_send_at("AT+CSQ\r\n", "OK", 800, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 6:
				ret = gprs_send_at("AT+CREG?\r\n", "OK", 800, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 7:
				gprs_status++;
//				ret = gprs_send_at("AT+CIPHEAD=1\r\n", "OK", 800, 10000);//
//				if (ret != NULL)
//				{
//					gprs_status++;
//					gprs_err_cnt = 0;
//				}
//				else
//				{
//					gprs_err_cnt++;
//					if (gprs_err_cnt > 5)
//					{
//						gprs_status = 0;
//					}
//				}
			break;
			
			case 8:
				ret = gprs_send_at("AT+CIPSTART=\"TCP\",\"103.46.128.47\",14947\r\n", "CONNECT OK", 1000, 10000);//
//				ret = gprs_send_at("AT+CIPSTART=\"TCP\",\"118.31.69.148\",1883\r\n", "CONNECT OK", 1000, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 9:
				ret = gprs_send_at("AT+CIPHEAD=1\r\n", "OK", 800, 10000);//
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
				
				
			case 10:
//				timer_delay_1ms(1500);
//				mqtt_con = mqtt_connect();
//				if(1 == mqtt_con)
//				{
//					gprs_status = 255;
//					timer_delay_1ms(20);
//					USART_OUT(USART1, "mqtt_connect ok\r\n");
//				}	
				gprs_status = 255;
			break;
				
			case 255:	//gprs ��ʼ����ɺ�������ݴ���
				
//				gprs_recv_task_create();
			
			break;
				
				
			default:
			break;		
		}	//switch end	
		
		if(gprs_status == 255)
		{
			break;
		}
	} // while end
	
		
}


















