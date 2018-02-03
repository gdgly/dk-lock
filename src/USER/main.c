#include "sys.h"  
#include "delay.h"
#include "rtc.h"    
#include "gps.h"
#include "string.h"		
#include "stdlib.h"
#include "usart3.h"
#include "usart2.h"
#include "24cxx.h"
#include "myiic.h"
#include "adc.h"
#include "bsp.h"
#include "timer.h"
#include "gprs.h"
#include "usart.h"
#include "aes.h"
#include "led.h"
#include "app_md5.h"



//gps
char Gps_Longi_At24C[8];
char Gps_Lati_At24C[7];
char Gps_Longi_At24C_Read[8];
char Gps_Lati_At24C_Read[7];
u32 Gps_longi;
u32 Gps_Lati;
u8 USART1_TX_BUF[512]; 					//串口1,发送缓存区


u8 cipherText[512];
u8 receiveText[24];
u8 expressText[512];  
u8 aesKey[16];


u8 PublishLockBackbuf[35];
u8 PublishDataBatbuf[35];
u8 PublishDataGpsbuf[45];
u8 PublishDataSelfCheckbuf[35];

nmea_msg gpsx; 											//GPS信息

u8 longitudedtbuf[30];
u8 latitudedtbuf[30];
u8 dtbuf[50]; 

//响铃
u8 bell_flag=0;
//手动
extern u8 Hand_Flag;
//震动
u8 Shaking=0;
u8 Shaking_Alarm_Flag=0;
//电压
u16 Bat_V;
u16 Bat_Pre;
u8 Bat_Pre_Flag=0;

u8 *p1;
u8 *p2;
//topic:bell
u8 *p3;
u8 *p4;
u8 *p5;


u8 Lock_Open=0;
u8 Lock_Open_On=0;
u8 Lock_Open_Off=0;
u8 Lock_Open_Tim5s;
u8 Lock_Close=0;
u8 Lock_Close_On=0;
u8 Lock_Close_Off=0;
u8 Lock_Close_Tim5s=0;//障碍物报警关



u8 flag=4;
__self_calendar_obj self_calendar;	//自检日历结构体

////////////////////////////////////
 u8 second_flag=0;
 u8 LOCK_ON_flag=1;
 u8 LOCK_OFF_flag=1;

u8 lockbuf[16];
u8 lockbuff[16];




u8 protocol_buff[512] = {0};


extern u8 PARK_LOCK_Buffer[16];
extern uint8_t gprs_status;

extern u8 usart2_buff[512];
extern u16 usart2_cnt; 

u8 tmp[20];

void Gps_Msg_Show(void)
{
 	float tp;		   
 
	tp=gpsx.longitude;	   
	sprintf((char *)longitudedtbuf,"%clongitude%c:%.5f %1c",'"','"',tp/=100000,gpsx.ewhemi);	
	//USART_OUT("%s\r\n",dtbuf);	 	
	USART_OUT(USART1,"%s\r\n",longitudedtbuf);	
	tp=gpsx.latitude;	   
	sprintf((char *)latitudedtbuf,"%clatitude%c:%.5f %1c",'"','"',tp/=100000,gpsx.nshemi);	
	USART_OUT(USART1,"%s\r\n",latitudedtbuf);
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			
}



int main(void)
{ 
	u16 j,i,rxlen;
	u16 k=0;
	u8 upload=0; 
	u8 *ret;
//	u8 *ret1, ret2;
	
	
	
	
	bsp_init();


	USART_OUT(USART1, "uart1 is ok\r\n");
	
	
	USART_OUT(USART1, "wangzhongya\r\n");
	
	MOTOA_HIGH();
	MOTOB_HIGH();

	while(1)
	{	 

		while(1)
		{
			gprs_config();
			if(gprs_status == 255)
			{
				break;
			}
		}
		usart1_recv_data();
		usart2_recv_data();
		
		
		
		MOTOA_LOW();
		timer_delay_1ms(200);
		MOTOB_HIGH();
		
		
		MakeFile_MD5_Checksum(PARK_LOCK_Buffer, 16);
	
		// 电池信息
		Bat_V =Get_Adc_Average(ADC_Channel_0,10);
		Bat_V=Bat_V*3300/4096;
		Bat_V=Bat_V*88/20;
		Bat_Pre=(Bat_V-5000)*100/2400;
		
		if(timer_is_timeout_1ms(tim1_cnt, 30000) == 0)
		{
			
 		
			sprintf((char *)PublishDataBatbuf,"%s%s%s","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",1,2\r\n");
			USART_OUT(USART1, "ssss=%s\r\n", PublishDataBatbuf);
			ret = gprs_send_at(PublishDataBatbuf, ">", 300, 0);
			if(ret != NULL)
			{
				memset(expressText, 0 ,512);
				memset(cipherText, 0 ,512);
				sprintf((char *)expressText,"{%c%s%c:%s}",'"',"battery",'"',"20");
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
//				AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
				USART_OUT(USART1, "aesKey=%s\r\n", aesKey);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
				memset(tmp, 0, 20);
				cipherText[0] = 0x31;
				tmp[0] = 0x31;
				ret = gprs_send_at(tmp, "OK", 300, 0);
				if(ret != NULL)
				{
					
				}
				memset(PublishDataBatbuf, 0, 512);
			}
//			
			if(Bat_Pre<10&&Bat_Pre_Flag==1)
			{
				Bat_Pre_Flag=0;
				sprintf((char *)PublishDataBatbuf,"%s%s%","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",36,2\r\n");
				USART_OUT(USART1, "wangzhongya=%s\r\n", PublishDataBatbuf);
				ret = gprs_send_at(PublishDataBatbuf, ">", 300, 0);
				if(ret != NULL)
				{
					memset(expressText, 0 ,512);
					memset(cipherText, 0 ,512);
					sprintf((char *)expressText,"{%c%s%c:%s}",'"',"battery",'"',"20");
//					AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
					ret = gprs_send_at(cipherText, "OK", 300, 0);
					if(ret == NULL)
					{
						
					}
				}
			}
		}
//		///
		p1 = strstr((u8*)protocol_buff, "topic: lock/");
		p2 = strstr((u8 *)p1,(u8 *)PARK_LOCK_Buffer);
		if(strncmp((char *)p1,(char *)"topic: lock/",12)==0)
		{
			USART_OUT(USART1, "lock\r\n");
			
 			if((ALARM_LOCK_ON() == 0) || (ALARM_LOCK_OFF() == 0))
			{
				memset(receiveText ,0 , 512);
				memset(expressText ,0 , 512);
				
				strcpy((char*)receiveText ,(char *)(p1+32));
//				AES_Decrypt((char*)expressText, (char*)receiveText, (char*)aesKey);
				Shaking=1;
				if(*receiveText==0x31)
				{
					if(ALARM_LOCK_ON() == 0)
					{
						Lock_Open=1;
						Lock_Open_On=1;
						
					}
					else
					{
						Lock_Open=0;
					}
				}
				else if(*receiveText==0x32)
				{
					if(ALARM_LOCK_OFF() == 0)
					{
						Lock_Close=1;
						Lock_Close_On=1;
					}
					else
					{
						Lock_Close=0;
					}
				
				}
				else if(*receiveText == 0x30)
				{
					Park_Lock_stop();
				}
				
				memset(protocol_buff, 0, 512);	
				
			}
		}
//		//
//		if((ALARM_LOCK_ON() == 1) || (ALARM_LOCK_OFF() == 1))
//		{
//			
//		}
//		
		p1 = strstr((char*)protocol_buff, "topic: bell/");
		p2 = strstr((char *)p1,(char *)PARK_LOCK_Buffer);
		if(strncmp((char *)p1,(char *)"topic: bell/",12)==0 && bell_flag==0)
		{
			USART_OUT(USART1, "bell\r\n");
		
			
			bell_flag=1;
			
			BEEP_HIGH();
			timer_delay_1ms(500);
			BEEP_LOW();
		}
		
		
		if(bell_flag == 1)
		{
			bell_flag = 0;
		}
		
		
		if(Lock_Open==1) 
		{
			Lock_Open=0;
			
			Park_Lock_Open();
		}
		
		if(Lock_Close == 1)
		{
			Lock_Close=0;
		
			Park_Lock_Close();
			
		}
		
		
		
		
		if(LOCK_ON_READ()==1 && LOCK_OFF_READ()==0 && Lock_Open == 1)
		{
			
			Park_Lock_Open();	//开锁
			if(LOCK_ON_READ()==0 && LOCK_OFF_READ()==1)
			{
				Lock_Open = 0;
				Park_Lock_stop();	//停止运行
				
				sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
				USART_OUT(USART1, "PublishLockBackbuf=%s\r\n", PublishLockBackbuf);
				ret = gprs_send_at(PublishLockBackbuf, ">", 300, 0);
				if(ret != NULL)
				{
					memset(expressText, 0 ,512);
					memset(cipherText, 0 ,512);
					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"1");
					USART_OUT(USART1, "expressText=%s\r\n", expressText);
//						AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
					ret = gprs_send_at(expressText, "OK", 300, 0);
					if(ret == NULL)
					{
						
					}
				}
				
			}
			
		}
		
		if(LOCK_ON_READ()==0 && LOCK_OFF_READ()==1 && Lock_Close == 1)
		{
			
			Park_Lock_Close();
			
			if(LOCK_ON_READ()==1 && LOCK_OFF_READ()==0)
			{
				Lock_Close = 0;
				Park_Lock_stop();	//停止运行
				
				sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
				USART_OUT(USART1, "PublishLockBackbuf=%s\r\n", PublishLockBackbuf);
				ret = gprs_send_at(PublishLockBackbuf, ">", 300, 0);
				if(ret != NULL)
				{
					memset(expressText, 0 ,512);
					memset(cipherText, 0 ,512);
					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"0");
					USART_OUT(USART1, "expressText=%s\r\n", expressText);
//						AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
					ret = gprs_send_at(expressText, "OK", 300, 0);
					if(ret == NULL)
					{
						
					}
				}
			}
		}
		
		if(LOCK_ON_READ()==0)
		{
//			BEEP_HIGH();
//			timer_delay_1ms(100);
//			BEEP_LOW();
			
//			USART_OUT(USART1, "ddddd\r\n");
		}
		
		
//		
//		if(ALARM_LOCK_ON()==0)
//		{
//			 Shaking=0;
//			
//			
//		}
//		
		
//		if((ALARM_LOCK_ON()==0)&&(LOCK_OFF_flag==0)&&(flag==4)&&Hand_Flag==0)
//		{
//			Shaking=0;
//			Park_Lock_stop();
//			
//			if(Shaking_Alarm_Flag==0)
//			{
//				if(Lock_Open_Off==1)
//				{
//					sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
//					USART_OUT(USART1, "PublishLockBackbuf=%s\r\n", PublishLockBackbuf);
//					ret = gprs_send_at(PublishLockBackbuf, ">", 300, 0);
//					if(ret != NULL)
//					{
//						memset(expressText, 0 ,512);
//						memset(cipherText, 0 ,512);
//						sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"1");
//						USART_OUT(USART1, "expressText=%s\r\n", expressText);
////						AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
//						ret = gprs_send_at(expressText, "OK", 300, 0);
//						if(ret == NULL)
//						{
//							
//						}
//					}
//				}
//				if(Lock_Open_Off==0)
//				{
//					sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
//					USART_OUT(USART1, "PublishLockBackbuf=%s\r\n", PublishLockBackbuf);
//					ret = gprs_send_at(PublishLockBackbuf, ">", 300, 0);
//					if(ret != NULL)
//					{
//						memset(expressText, 0 ,512);
//						memset(cipherText, 0 ,512);
//						sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"0");
//						USART_OUT(USART1, "expressText=%s\r\n", expressText);
////						AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
//						ret = gprs_send_at(expressText, "OK", 300, 0);
//						if(ret == NULL)
//						{
//							
//						}
//					}
//				}				
//			}
//			Lock_Open_Off=0;
//			Lock_Open_On=0;
//			Lock_Close_On=0;
//			LOCK_OFF_flag=1;
//		}
//		else if(ALARM_LOCK_ON()==1)
//		{
//			LOCK_OFF_flag=0;
//		}
//		
//		if(Lock_Close_On == 1)
//		{
//			
//		}
		
	}	 
}
 
























