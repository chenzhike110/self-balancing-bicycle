#ifndef __TIMER_H
#define __TIMER_H
#include <sys.h>	 

void Timer1_Init(u16 arr,u16 psc);
void Timer2_Init(u16 arr,u16 psc);

extern u8 server_pwm;
#endif


