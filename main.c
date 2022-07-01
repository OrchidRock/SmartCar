// This is a project that tay to implements the smart car.
// 2016-12-1
// 2022-5-20

#include <stdio.h>
#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "uart0_debug.h"
#include "uart1.h"
#include "Ov7670/ov7670.h"
#include "systick_delay.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);

void RGB_LED_Init(void); // Init the pin PF1,PF2,PF3

void DelayTimer_Init(void); // Timer1A;
void DelayTimer_Wait(unsigned long); 



// ***** 3. Main function section *****

// ***** 4. Subroutines Section *****


void RGB_LED_Init(){
        volatile unsigned long delay;
        // port PF1
        SYSCTL_RCGC2_R |= 0x00000020; // activate port F
        delay = SYSCTL_RCGC2_R;
        GPIO_PORTF_CR_R |= 0x0e; // allow change for PF1,2,3
        GPIO_PORTF_AMSEL_R &= ~0x0e;
        GPIO_PORTF_DIR_R |= 0x0e; // PF1,2,3 out
        GPIO_PORTF_AFSEL_R &= ~0x0e;
        GPIO_PORTF_DEN_R |= 0x0e; // enable digital I/O on PF1,2,3
        GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+
                0x00000000;		
        while(delay--){};
}

int main(void){ 
        PLL_Init(Bus80MHz); // 80MHZ
        RGB_LED_Init();
        systick_delay_init();
        UART0_Init();       // debug
        OV7670_init();
        //UART1_Init();       // uart for nodemcu 
        EnableInterrupts();
        
        uint32_t value;
        uint32_t timer_count = 0;

        while(1){
            timer_count = 0;
            //PWM0A_Disable();
            UART0_OutStringCRLF("Get Raw");            
            while(VSYNC_STATE_H) {} //value = DATA_STATE;
            while(VSYNC_STATE_L) {}// value = DATA_STATE;
            PWM0A_Enable();
            
            while(timer_count < 76800){
                if (HREF_STATE) {
                    value = DATA_STATE;
                    UART0_OutChar(value);
                    timer_count ++;
                }else{
                    UART0_OutChar(CR);UART0_OutChar(LF); 
                }
            }


        }
}
