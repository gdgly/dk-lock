#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define YELLOW_LED PCout(2)	// PA12
#define BEEP PCout(3)
#define GPS_POW_EN PCout(4)

#define MOTOR_A PCout(0)	// Pc7	
#define MOTOR_B PCout(1)	// Pc8		

void LED_BEEP_MOTO_Init(void);//≥ı ºªØ
void Park_Lock_Open(void);
void Park_Lock_Close(void);	 	
void Park_Lock_stop(void);
#endif
