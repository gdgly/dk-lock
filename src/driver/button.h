

#ifndef __BUTTON_H_
#define __BUTTON_H_
#include <stm32f10x.h>


typedef enum
{
	buuton_null,
	button_up,
	button_click,
	button_long_press
	
} button_state_e;

typedef struct
{
	button_state_e  butt_state;
	u8 				butt_en;
	u8				butt_cnt;
	u32				butt_long_press_timer;

} button_info_s;








#define BUTTON_VALID_CNT			10
#define BUTTON_SUM					1

void button_timer_ms(void);
button_state_e button_get_state(u8 button_name, u32 long_time);


#endif
