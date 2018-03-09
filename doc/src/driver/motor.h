


#ifndef __MOTOR_H_
#define __MOTOR_H_

#include  "bsp.h"
#include <stm32f10x.h>






void lock_open(void);
void lock_close(void);
void lock_stop(void);
u8 lock_on_status_get(void);
u8 lock_off_status_get(void);

#endif
