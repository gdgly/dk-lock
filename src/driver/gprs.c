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




extern usart_buff_t  *usart2_rx_buff;


extern u8 usart2_buff[512];
extern u16 usart2_cnt;

extern u8 usart2_rx_status;





uint8_t gprs_err_cnt = 0; 	//GPRS错误计数器
uint8_t gprs_status = 0;	//GPRS的状态

uint8_t gprs_rx_buf[512];
uint16_t gprs_rx_cnt = 0;

uint8_t gprs_send_at_flag = 0;	
uint8_t gprs_rx_flag = 0;



u8 PARK_LOCK_Buffer[16] = {0};
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
* Description : 检测字符串中是否存在子字符串.
*
* Argument(s) : p_str ： 要检测的子字符串
*
* Return(s)   : 返回子字符串的位置指针
*
* Caller(s)   : gprs_send_at()
*
* Note(s)     : 
*********************************************************************************************************
*/
u8 *gprs_check_cmd(u8 *p_str)
{
	char *str = 0;

	str = strstr((const char*)usart2_buff, (const char*)p_str);
	
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
	u8 *buff;
	
	timer_is_timeout_1ms(timer_at, 0);	//开始定时器timer_at
	while (res)
	{
		
		memset(usart2_buff, 0, 512);
		usart2_cnt = 0;
		
		usart2_rx_status = 0;
		USART_OUT(USART2, cmd);							
		timer_delay_1ms(waittime);				//AT指令延时

		USART_OUT(USART1, usart2_buff);
		usart2_rx_status = 1;	//数据未处理 不在接收数据
		
		if (gprs_check_cmd(ack))	
		{
			res = 0;				//监测到正确的应答数据
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			memcpy(buff, usart2_buff, 512);
			memset(usart2_buff, 0, 512);
			usart2_cnt = 0;		
			return buff;
		}
			
		if (timer_is_timeout_1ms(timer_at, timeout) == 0)	//定时器timer_at结束
		{
			res = 0;
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			return NULL;
		}
		
	}
	
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
				if(strstr((char*)ret, "+STATUS: MQTT READY") != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else if(strstr((char*)ret, "+STATUS: MQTT CONNECT OK") != NULL)
				{
					gprs_status = 4;
					gprs_err_cnt = 0;
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
			ret = gprs_send_at("AT+STATUS\r\n", "+STATUS: MQTT CONNECT OK", 800,5000);
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
				memcpy(PARK_LOCK_Buffer, str+12, 16);	//读出锁id	
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
			ret = gprs_send_at("AT+STATUS\r\n", "+STATUS: MQTT CONNECT OK", 800,5000);
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
			usart2_rx_status = 2;
			USART_OUT(USART1, "usart2_rx_status = %d\r\n", usart2_rx_status);
				
			ret = gprs_send_at("AT+CLIENTID?\r\n", "OK", 50, 1000);
			if (ret != NULL)
			{
				USART_OUT(USART1, ret);
				str = strstr((const char*)ret, (const char*)"+CLIENTID: ");
				memcpy(PARK_LOCK_Buffer, str+12, 16);	//读出锁id	
				USART_OUT(USART1, "lock_id=%s\r\n", PARK_LOCK_Buffer);
				gprs_status=255;
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
//			USART_OUT(USART1, "GPRS INIT OK\r\n");
//		USART_OUT(USART1, "GPRS INIT OK = %d\r\n", usart2_rx_status);
		break;	
		
		default:
		break;				
				
	}
	
	
	
	
}







/*
*********************************************************************************************************
*                                          gprs_init_task_fun()
*
* Description : GPRS初始化任务函数.
*
* Argument(s) : p_arg :该任务要传的参数
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void gprs_init_task_fun(void *p_arg)
{
//	OS_ERR err;
//	OS_MSG_SIZE size = 0;
	u8 *msg;
	u8 size1;
	u8 *ret;
	static u8 gprs_init_flag = true;		//
		
	while(1)
	{
		
		switch(gprs_status)
		{
			case 0:
//				gprs_power_on();
//				OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
				gprs_status = 1;
				gprs_err_cnt = 0;
			break;
					
			case 1:
				ret = gprs_send_at("\r\nAT\r\n", "OK", 800,10000);
				if (ret == 0)
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
				ret = gprs_send_at("\r\nATI\r\n", "OK", 800, 10000);
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT+CPIN?\r\n", "OK", 800, 10000);
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT+CREG=1\r\n", "OK", 800, 10000);
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT+CSQ\r\n", "OK", 800, 10000);
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT+CREG?\r\n", "OK", 800, 10000);
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT^SICS=0,conType,GPRS0\r\n", "OK", 800, 10000);//?¨á￠á??óProfile éè??conType
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT^SICS=0,APN,CMNET\r\n", "OK", 800, 10000);//
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT^SISS=0,srvType,Socket\r\n", "OK", 800, 10000);//
				if (ret == 0)
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
				ret = gprs_send_at("\r\nAT^SISS=0,conId,0\r\n", "OK", 800, 10000);//
				if (ret == 0)
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
					
			case 11:
				ret = gprs_send_at("\r\nAT^SISS=0,address,\"socktcp://180.169.14.34:16650\"\r\n", "OK", 800, 10000);//
				if (ret == 0)
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
			
			case 12:
				ret = gprs_send_at("\r\nAT^SISO=0\r\n", "OK", 5000, 20000);//
				if (ret == 0)
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
			
			case 13:
				ret = gprs_send_at("\r\nAT^IPCFL=5,20\r\n", "OK", 800, 10000);//
				if (ret == 0)
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
			
			case 14:
				ret = gprs_send_at("\r\nAT^IPENTRANS=0\r\n", "OK", 800, 10000);//
				if (ret == 0)
				{
					gprs_status = 255;
					gprs_init_flag = false;
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
				
			case 255:	//gprs 初始化完成后进行数据传输
				
//				gprs_recv_task_create();
			
			break;
				
				
			default:
			break;		
		}	//switch end			
	} // while end
	
		
}





/*
*********************************************************************************************************
*                                          gprs_recv_task_fun()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void gprs_recv_task_fun(void *p_arg)
{
//	OS_ERR err;



	while(1)
	{	

		usart2_recv_data();
		
	}
		
}












