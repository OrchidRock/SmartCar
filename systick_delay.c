
#include <stdint.h>
#include "systick_delay.h"

void SysTick_Wait(uint32_t delay){
    NVIC_ST_RELOAD_R = delay - 1;
    NVIC_ST_CURRENT_R = 0;
    while((NVIC_ST_CTRL_R & 0x00010000)==0){
         
    }
}

void systick_delay_init(void){
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_CTRL_R = 0x00000005;
}

void systick_delay_ms(uint32_t delay){
    uint32_t i = 0;
    for (i=0; i < delay; i++){
        SysTick_Wait(80000); 
    }
}

void systick_delay_us(uint32_t delay){
    uint32_t i = 0;
    for (i=0; i < delay; i++){
        SysTick_Wait(80); 
    }
    
}
