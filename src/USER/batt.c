


#include "batt.h"
#include "adc.h"
#include <stdio.h>
#include <string.h>
#include "aes.h"
#include "usart.h"
#include "gprs.h"





extern u8 PARK_LOCK_Buffer[16];

u8 batt_state = 0;
extern u8 aesKey[16];


u16 batt_get_volume(void)
{
	
	
	u16  batt_value = 0, res = 0;
	
//	batt_value = adc_average_value(ADC_Channel_0, 10);
	batt_value = Get_Adc_Average(ADC_Channel_0, 10);
	
	batt_value = batt_value * 3300 / 4096;
	
	batt_value = batt_value * 88 / 20;
	
	res = (batt_value-5000) * 100 / 1024;
	

	return res;
}

void batt_dev_to_svr(void)
{
	u16 val = 0;
	
	u8 publish_buff[512] = {0};
	u8 tmp[512] = {0};
	u8 aes_data[512] = {0};
	
	val = batt_get_volume();
	
	if(val < 20 && val > 10 && batt_state == 0)
	{
		batt_state = 1;
		sprintf((char *)publish_buff, "%s%s%s", "AT+PUBLISH=lockdata/", PARK_LOCK_Buffer, ",24,2\r\n");
				
		gprs_send_at(publish_buff, ">", 30, 0);
		memset(tmp, 0 , 512);
		memset(aes_data, 0 ,512);
		sprintf((char *)tmp, "{%c%s%c:%s}", '"',"battery", '"', "20");
		AES_Encrypt((char *)tmp, (char*)aes_data, (char*)aesKey);
		gprs_send_at(aes_data, "OK", 30, 0);
		
	}
	
	
	if(val < 0 && batt_state == 1)
	{
		batt_state = 0;
		
		sprintf((char *)publish_buff, "%s%s%s", "AT+PUBLISH=lockdata/", PARK_LOCK_Buffer, ",24,2\r\n");
				
		gprs_send_at(publish_buff, ">", 30, 0);
		memset(tmp, 0 , 512);
		memset(aes_data, 0 ,512);
		sprintf((char *)tmp, "{%c%s%c:%s}", '"',"battery", '"', "10");
		AES_Encrypt((char *)tmp, (char*)aes_data, (char*)aesKey);
		gprs_send_at(aes_data, "OK", 30, 0);
	}
	
}




