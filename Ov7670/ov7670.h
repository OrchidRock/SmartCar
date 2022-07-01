#ifndef _OV7660_H
#define _OV7660_H


#include "SCCB.h"
#include "../tm4c123gh6pm.h"

#define DATA_STATE ((GPIO_PORTC_DATA_R & 0xf0) | (GPIO_PORTD_DATA_R & 0x0f))

#define HREF_BIT    PA5
#define XCLK_BIT    PA6 // PWM
#define VSYNC_BIT   PA4
#define PCLK_BIT    PA3

#define XCLK_H	        GPIOA->BSRR =  GPIO_Pin_8;
#define XCLK_L		    GPIOA->BRR =   GPIO_Pin_8;

#define PCLK_STATE    (GPIO_PORTA_DATA_R & 0x00000008) // PA3
#define HREF_STATE    (GPIO_PORTA_DATA_R & 0x00000020) // PA5
#define VSYNC_STATE_H (GPIO_PORTA_DATA_R & 0x00000010) // PA4 
#define VSYNC_STATE_L (GPIO_PORTA_DATA_R ^ 0x00000010) // PA4 

#define GPIOC_CRL		    GPIOC->BRR =0x0000; 

/////////////////////////////////////////
void CLK_init_ON(void);
void CLK_init_OFF(void);
unsigned char wrOV7670Reg(unsigned char regID, unsigned char regDat);
unsigned char rdOV7670Reg(unsigned char regID, unsigned char *regDat);
void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height);
unsigned char OV7670_init(void);
void ov7670_GPIO_Init(void);
//void ov7660_GPIO_CONTRL_CONFIG(void)

#endif /* _OV7660_H */



