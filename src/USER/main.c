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


//gps
char Gps_Longi_At24C[8];
char Gps_Lati_At24C[7];
char Gps_Longi_At24C_Read[8];
char Gps_Lati_At24C_Read[7];
u32 Gps_longi;
u32 Gps_Lati;
u8 USART1_TX_BUF[512]; 					//串口1,发送缓存区


char cipherText[512];
char receiveText[24];
char expressText[512];  
char aesKey[16];


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


extern u8 PARK_LOCK_Buffer[16];
extern uint8_t gprs_status;


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
	bsp_init();


	USART_OUT(USART1, "uart1 is ok\r\n");
	
	
	USART_OUT(USART1, "wangzhongya\r\n");
	

//	while(1)
//	{
//		USART_OUT(USART1, "123465\r\n");
//		if(USART3_RX_STA&0X8000)		
//		{
//			rxlen=USART3_RX_STA&0X7FFF;	
//			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];	   
//			
//			USART3_RX_STA=0;		   	
//			USART1_TX_BUF[i]=0;			
//			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);
//			Gps_Msg_Show();		
//			if(upload)USART_OUT(USART1,"\r\n%s\r\n",USART1_TX_BUF);
//			
//			if(gpsx.latitude>0&&gpsx.longitude>0)
//			{
//					memset(expressText, 0 ,1024);
//					sprintf((char *)expressText,"%c%s,%s%c",'{',(char *)longitudedtbuf,(char *)latitudedtbuf,'}',48);
//					sprintf((char *)PublishDataGpsbuf,"%s%s%s","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",64,2\r\n");	
////				    SendCmd((char *)PublishDataGpsbuf,">",20);
//					gprs_send_at(PublishDataGpsbuf, ">", 200, 10000);
////					myfree(PublishDataGpsbuf);				
//					//SendCmd((char *)localbuf,"OK",10);//
//					memset(cipherText, 0 ,1024);
//					AES_Encrypt((char *)expressText, cipherText, aesKey);
//					SendCmd((char *)cipherText,"OK",20);//
//					USART_OUT(USART1, "local:%s\r\n",expressText);	
//				  Gps_longi=gpsx.longitude;
//				  Gps_Lati=gpsx.latitude;
//				break;

//				 
//			}
//		}
//		else
//		{
//			k++;
//			timer_delay_1ms(1000);
//		//	BEEP=1;
//			if(k>10)
//			{
//				break;
//			}
//		}
//		usart1_recv_data();

//		usart2_recv_data();	
//  }

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
		
		
		// 电池信息
//		Bat_V =Get_Adc_Average(ADC_Channel_0,10);
//		Bat_V=Bat_V*3300/4096;
//		Bat_V=Bat_V*88/20;
//		Bat_Pre=(Bat_V-5000)*100/2400;
//		if(Bat_Pre<20&&Bat_Pre>10&&Bat_Pre_Flag==0)
//		{
//			Bat_Pre_Flag=1;
//			sprintf((char *)PublishDataBatbuf,"%s%s%s","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",24,2\r\n");
//			SendCmd((char *)PublishDataBatbuf,">",20);
//			timer_delay_1ms(10);
//			memset(expressText, 0 ,1024);
//			sprintf((char *)expressText,"{%c%s%c:%s}",'"',"battery",'"',"20");
//			memset(cipherText, 0 ,1024);
//			AES_Encrypt((char *)expressText, cipherText, aesKey);
//			SendCmd((char *)cipherText,"OK",20);//
//			//SendCmd((char *)bat_buff,"OK",20);//
//		}
//		
//		if(Bat_Pre<10&&Bat_Pre_Flag==1)
//		{
//			Bat_Pre_Flag=0;
//			
//			sprintf((char *)PublishDataBatbuf,"%s%s%s","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",24,2\r\n");
//			SendCmd((char *)PublishDataBatbuf,">",20);
//			timer_delay_1ms(10);
//			memset(expressText, 0 ,1024);
//			sprintf((char *)expressText,"{%c%s%c:%s}",'"',"battery",'"',"10");
//			memset(cipherText, 0 ,1024);
//			AES_Encrypt((char *)expressText, cipherText, aesKey);
//			SendCmd((char *)cipherText,"OK",20);//
//			
//		}
		
//		
//		p1=strstr((u8 *)usart2_rcv_buf,"topic: lock/");
//		p2=strstr((u8 *)p1,(u8 *)PARK_LOCK_Buffer);

////		USART_OUT(USART1,"usart_p1:%s\r\n",p1);
//		
////		USART_OUT(USART1, "usart_p2:%s\r\n",p2);
//		if(strncmp((char *)p1,(char *)"topic: lock/",12)==0)
//		{		
//			if(LOCK_OFF==0||LOCK_ON==0)
//			{			
//		if(strncmp((char *)p1,(char *)"topic: lock/",12)==0)
//		{	
//			USART_OUT(USART1, "p1:%s\r\n",p1+33);
//			strcpy(receiveText ,(char *)(p1+33));
//			USART_OUT(USART1, "ci:%s\r\n",receiveText);
//			memset(expressText ,0 ,1024);
//			AES_Decrypt(expressText ,receiveText, aesKey);
//			Shaking=1;	
//		    USART_OUT(USART1, "ex:%s\r\n",expressText);
//				USART_OUT(USART1, "aeskey:%s\r\n",aesKey);
//				
//					if(*expressText==0x31)
//					{
////						USART_OUT("p2:%s\r\n",p2);	
//						if(LOCK_ON==0)
//						{
//						USART_OUT(USART1, "p2:%s\r\n",p2);	
//						Lock_Open=1; 
//						Lock_Open_Tim5s=calendar.sec;
//						Lock_Open_On=1;
//						}
//						else
//						{
//							Lock_Open=0;
//						}
//					}
//					else if(*expressText==0x32)
//					{
//						if(LOCK_OFF==0)
//						{
//						Lock_Close=1;
//						Lock_Close_Tim5s=calendar.sec;
//						Lock_Close_On=1;
//						}
//						else
//						{
//						Lock_Close=0;
//						}
//					}	 
//					else if(*expressText==0x30)
//					{
//						Park_Lock_stop();
//					}	
//			USART2_Clear();
//			p1="\0";		
////			myfree(p2);
////			myfree(p4);
////			myfree(p3);
////			myfree(p1);	
//			}
//			}
//		}
//		
//		if(LOCK_OFF==1||LOCK_ON==1)
//		{
//			if(strncmp((char *)p1,"topic: lock",11)==0)
//			{	
//					USART2_Clear();
//					p1="\0";		
////					myfree(p2);
////					myfree(p1);	
//			}
//		}
//		
//		p3=strstr((char *)usart2_rcv_buf,"topic: bell/");
////		USART_OUT(USART1, "usart_p3:%s\r\n",p3);
//		p4=strstr((char *)p3,(char *)PARK_LOCK_Buffer);
//				
////		USART_OUT(USART1, "usart_p4:%s\r\n",p4);
//	//if(strncmp((char *)p4,(char *)PARK_LOCK_Buffer,8)==0)					
//	//p1=strstr((char *)usart2_rcv_buf,"topic");
//		if(strncmp((char *)p3,"topic: bell/",12)==0&&bell_flag==0)
//		//if(strncmp((char *)p4,(char *)PARK_LOCK_Buffer,16)==0&&bell_flag==0)
//		{		
//			bell_flag=1;
//			USART_OUT(USART1, "p1:%s\r\n",p4);			
////			YELLOW_LED=1;
////			BEEP=1;
//			BEEP_HIGH();

//			//p1="\0";
//			USART2_Clear();
////
//			
//			timer_delay_1ms(2000);
////			YELLOW_LED=0;
////			BEEP=0;
//			BEEP_LOW();
//		}
//	  if(bell_flag==1)
//		{
//			bell_flag=0;
//		}
//		  //USART2_Clear();
//			//myfree(usart2_rcv_buf);
//			//myfree(p1);
//			//myfree(p2);
//			//myfree(p3);
//			//myfree(p4);
//			//usart2_rcv_len=0;
//			timer_delay_1ms(10);


//		if(Lock_Open==1) 
//		{
//			Lock_Open=0;
//			
//			Park_Lock_Open();
//		}

//    if(Lock_Close==1)  
//		{
//			Lock_Close=0;
//		
//			Park_Lock_Close();
//		}
//		

//		//key=KEY_Scan(0);
//		if((LOCK_ON==0)&&(LOCK_OFF_flag==0)&&(flag==4)&&Hand_Flag==0)
//		{		
//      Shaking=0;			
//			self_calendar.self_w_date=calendar.w_date;
//			self_calendar.self_hour=calendar.hour;
//			self_calendar.self_min=calendar.min;
//		  self_calendar.self_flag=0;
//			//IWDG_Feed();	
//			
//		  Park_Lock_stop();
//			if(Shaking_Alarm_Flag==0)
//			{
//				if(Lock_Open_Off==1)
//				{
//					sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");	
//					SendCmd((char *)PublishLockBackbuf,">",20);//

////					myfree(PublishLockBackbuf);
////					timer_delay_1ms(30);
//					//SendCmd("AT+PUBLISH=lockback/00010002,16,2\r\n",">",20);
//					memset(expressText , 0 ,1024);
//					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"1");
//					memset(cipherText, 0 ,1024);
//          AES_Encrypt((char *)expressText, cipherText, aesKey);
//					SendCmd((char *)cipherText,"OK",20);//
//					//SendCmd((char *)lockbuf,"OK",20);//
//				}
//				if(Lock_Open_Off==0)
//				{
//					sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");		
//					SendCmd((char *)PublishLockBackbuf,">",20);//
////						myfree(PublishLockBackbuf);
////					timer_delay_1ms(30);
//				//	SendCmd("AT+PUBLISH=lockback/00010002,16,2\r\n",">",20);
//					memset(expressText , 0 ,1024);
//					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"0");//10
//					USART_OUT(USART1, "cmd:%s\r\n",expressText);
//					memset(cipherText , 0 ,1024);
//					AES_Encrypt((char *)expressText, cipherText, aesKey);
//					SendCmd((char *)cipherText,"OK",20);//
//					//SendCmd((char *)lockbuff,"OK",20);//
////					BEEP=1;
//					BEEP_HIGH();
//					timer_delay_1ms(20);
//					BEEP_LOW();
//				}
//		  }
//			Lock_Open_Off=0;
//			Lock_Open_On=0;
//			Lock_Close_On=0;
//			USART_OUT(USART1, "lockback_on\r\n");
//			
////			myfree((char *)lockbuf);
////			myfree((char *)lockbuff);
//			LOCK_OFF_flag=1;
//		}
//		else if(LOCK_ON==1)
//		{
//			LOCK_OFF_flag=0;
//		}
//		if(Lock_Close_On==1)
//		{
//			u8 temp=calendar.sec;
//			if(temp>=Lock_Close_Tim5s)
//			{
//				if((temp-Lock_Close_Tim5s)>5)//关锁超时
//				{
//					Lock_Close_On=0;
//					Park_Lock_Open();
//					Lock_Close_Off=1;
//				}
//			}
//			else
//			{
//				if((temp+60-Lock_Close_Tim5s)>5)//关锁超时
//				{
//					Lock_Close_On=0;
//					Park_Lock_Open();
//					Lock_Close_Off=1;
//				}		
//			}		
//		}
////			IWDG_Feed();	
//		//开锁返回开锁1的返回命令
//	 if((LOCK_OFF==0)&&(LOCK_ON_flag==0)&&(flag==4))//正常开锁对应1
//		{
//			 Shaking=0;	//晃动排除	
//			//自检时间标定
//			self_calendar.self_w_date=calendar.w_date;
//			self_calendar.self_hour=calendar.hour;
//			self_calendar.self_min=calendar.min;
//			self_calendar.self_flag=0;

//			
//			Park_Lock_stop();
//			if(Shaking_Alarm_Flag==0)//晃动报警和自检
//			{
//				if(Lock_Close_Off==1)
//				{
//					sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");							
//	        SendCmd((char *)PublishLockBackbuf,">",20);//
////					myfree(PublishLockBackbuf);
//					//timer_delay_1ms(30);
//					memset(expressText , 0 ,1024);
//					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"1");	
//					
//					//SendCmd("AT+PUBLISH=lockback/00010002,16,2\r\n",">",20);
//					memset(cipherText , 0 ,1024);
//          AES_Encrypt((char *)expressText, cipherText, aesKey);
//					SendCmd((char *)cipherText,"OK",20);//
//					//SendCmd((char *)lockbuf,"OK",20);//
//				}
//				if(Lock_Close_Off==0)
//				{
//					sprintf((char *)PublishLockBackbuf,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
//			    SendCmd((char *)PublishLockBackbuf,">",20);//
////					myfree(PublishLockBackbuf);
//					
//				//	SendCmd("AT+PUBLISH=lockback/00010002,16,2\r\n",">",20);
//					memset(expressText , 0 ,1024);
//					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"0");
//					
//					memset(cipherText , 0 ,1024);
//          AES_Encrypt((char *)expressText, cipherText, aesKey);
//					SendCmd((char *)cipherText,"OK",20);//
//					//SendCmd((char *)lockbuff,"OK",20);//
//					USART_OUT(USART1, "ca_off:%d\r\n",calendar.sec);
////					BEEP=1;
//					BEEP_HIGH();
//					timer_delay_1ms(10);
////					BEEP=0;
//					BEEP_LOW();
//				}
//			}
//			Lock_Open_Off=0;
//			Lock_Open_On=0;
//			Lock_Close_Off=0;
//			Lock_Close_On=0;
//			//停止低电压
////			myfree((char *)lockbuf);
////			myfree((char *)lockbuff);
//			LOCK_ON_flag=1;
//		}
//		else if(LOCK_OFF==1)
//		{
//			LOCK_ON_flag=0;
//		}
//		
//		//超时检测
//		if(Lock_Open_On==1)
//		{
//			u8 temp=calendar.sec;
//			if(temp>=Lock_Open_Tim5s)
//			{
//				if((temp-Lock_Open_Tim5s)>3)//开锁超时
//				{
//					Lock_Open_On=0;
//					Park_Lock_Close();
//					Lock_Open_Off=1;
//				}
//			}
//			else
//			{
//				if((temp+60-Lock_Open_Tim5s)>3)//开锁超时
//				{
//					Lock_Open_On=0;
//					Park_Lock_Close();
//					Lock_Open_Off=1;
//				}		
//			}		
//		}
//		/*//	自检功能
//		if(calendar.w_date>self_calendar.self_w_date)
//		{
//		if((calendar.w_date-self_calendar.self_w_date==2)&&(self_calendar.self_flag==0)&&(calendar.sec==0)&&(calendar.min==0))//
//		{
//			Self_Check();
//			self_calendar.self_flag=1;
//			flag=0;
//		}	
//		}
//		timer_delay_1ms(10);*/
//		
//		
//		////////////发布信息地锁经纬度
//		//if(calendar.sec==0&&second_flag==0)//24小时检测一次
//		if(calendar.sec==0&&second_flag==0&&calendar.hour==0&&calendar.min==0)//24小时检测一次
//		{
////			GPS_POW_EN=1;
//			GPS_POW_HIGH();
//			second_flag=1;
//			if(USART3_RX_STA&0X8000)		
//			{
//				rxlen=USART3_RX_STA&0X7FFF;	
//				for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];	   
//				USART3_RX_STA=0;		   
//				USART1_TX_BUF[i]=0;			
//				GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);
//				Gps_Msg_Show();				
//				if(upload)USART_OUT(USART1,"\r\n%s\r\n",USART1_TX_BUF);
//			}
//			if(gpsx.latitude>0&&gpsx.longitude>0)
//				{
//					/*
//					myfree(Gps_Lati_At24C_Read);
//					myfree(Gps_Longi_At24C_Read);
//					USART_OUT("str:%s\r\n",(char *)Gps_Longi_At24C_Read);
//					AT24CXX_Read(0,(u8 *)Gps_Longi_At24C_Read,8);
//					AT24CXX_Read(1,(u8 *)Gps_Lati_At24C_Read,7);
//					USART_OUT("str:%s\r\n",(char *)Gps_Longi_At24C_Read);
//					Gps_longi=atol(Gps_Longi_At24C_Read);
//					Gps_Lati=atol(Gps_Lati_At24C_Read);
//					USART_OUT("12345:%ld\r\n",Gps_longi);
//					USART_OUT("6789:%ld\r\n",(long)gpsx.longitude);*/
////					if((labs(Gps_longi-gpsx.longitude)>5000)||(labs(Gps_Lati-gpsx.latitude)>5000))
////					{
//						memset(expressText, 0 ,1024);
//						sprintf((char *)expressText,"%c%s,%s%c",'{',(char *)longitudedtbuf,(char *)latitudedtbuf,'}',48);
//						sprintf((char *)PublishDataGpsbuf,"%s%s%s","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",64,2\r\n");
//						
//					//SendCmd("AT+PUBLISH=lockdata/00010002,78,2\r\n",">",10);		
//						SendCmd((char *)PublishDataGpsbuf,">",20);
////						myfree(PublishDataGpsbuf);				
//						//SendCmd((char *)localbuf,"OK",10);//
//						memset(cipherText, 0 ,1024);
//						AES_Encrypt((char *)expressText, cipherText, aesKey);
//						SendCmd((char *)cipherText,"OK",20);//
//						USART_OUT(USART1, "local:%s\r\n",expressText);	
////					}
////					GPS_POW_EN=0;
//					GPS_POW_LOW();
//		   }		
//			//Self_Check();

//				//USART_OUT("str:%s\r\n",expressText);
//		}
//		else if(calendar.sec!=0)
//		{
//			second_flag=0;	
//		}

////		USART_OUT(USART1, "sec:%d\r\n",calendar.sec);
//		Hand_Lock();

//		if((LOCK_OFF==1&&LOCK_ON==1)&&(Shaking==0))
//		{
//			//USART_OUT("123456");
//			Shaking_Alarm_Flag=1;
////			BEEP=1;
//			BEEP_HIGH();
//		}
//		else
//		{
//			Shaking_Alarm_Flag=0;
////			BEEP=0;
//			BEEP_LOW();
//		}
//		
		
		
	}	 
}
 
























