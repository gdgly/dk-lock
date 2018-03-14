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
#ifndef __GPRS_H_
#define __GPRS_H_

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include  "bsp.h"
#include <stm32f10x.h>
#include <stdbool.h>



typedef struct
{
	uint8_t *server_ip;	
	uint32_t server_port;
}GPRS_CONFIG;













void gprs_power_on(void);



u8 *gprs_send_at(u8 *cmd, u8 *ack, u16 waittime, u16 timeout);
uint8_t send_at_cmd(uint8_t *cmd, uint8_t *str, uint32_t timeout);
int gprs_send_data(u8 *data, u16 data_len, u8 *ack, u16 waittime);
uint8_t gprs_init(void);

void gprs_config(void);

void gprs_init_task(void);


#endif

