

#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "usart.h"


extern u8 usart2_buff[512];
extern u16 usart2_cnt;
extern u8 PARK_LOCK_Buffer[16];
extern u8 protocol_buff[512];


void protocol_analyze(void)
{
	u8 topic[100] = {0};
	u8 *par = NULL;
	usart2_recv_data();
		
	//topic: lock/0018131408143929,1
	par = strstr((char *)protocol_buff, "topic:");
	
	if(par != NULL)
	{
		memcpy(topic, "topic: lock/", 12);
	}
	
	sprintf((char*)topic, "%s%s", "topic: lock/", PARK_LOCK_Buffer);
	
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






