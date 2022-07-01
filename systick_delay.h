#ifndef __SYSTICK_DELAY__
#define __SYSTICK_DELAY__

#include <stdint.h>
#include "tm4c123gh6pm.h"

void systick_delay_init();

void systick_delay_ms(uint32_t delay);

void systick_delay_us(uint32_t delay);



#endif
