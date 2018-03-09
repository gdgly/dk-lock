

#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "usart.h"
#include "common.h"



extern u8 usart2_buff[512];
extern u16 usart2_cnt;
extern u8 PARK_LOCK_Buffer[17];
extern u8 protocol_buff[512];

s8 a;

void protocol_analyze(void)
{
	mqtt_telegram_t mt;
	
	u8 topic_tmp[100] = {0};
	u8 topic[100] = {0};
	u8 buff[100] = {0};
	u16 data_len = 0;
	u8 *par = NULL;
	usart2_recv_data();
		
	//topic: lock/0018131408143929,1
	par = strstr((char *)protocol_buff, "topic:");
	
	if(par != NULL)
	{
		par = str_picked(protocol_buff, "topic:", ",", mt.topic);
		if(par != NULL)
		{
			
		}
	}
	
	sprintf((char*)topic, "%s%s", "lock/", PARK_LOCK_Buffer);
	
	if(strcmp(topic_tmp, topic) == 0)
	{
		
	}
	if(strstr((char *)usart2_buff, "+STATUS: MQTT CLOSE") != NULL)
	{
		
	}
	else if(strstr((char *)usart2_buff, (char*)topic) != NULL)
	{
		if(LOCK_ON_READ()==0 || LOCK_OFF_READ() == 0)
		{
			
			
		}
	}


}






