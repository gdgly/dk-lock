



#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include  "bsp.h"
#include <stm32f10x.h>


typedef struct
{
	u16 pdata_len;
	u8 *topic;
	u8 *pdata;

} mqtt_telegram_t;













#endif
