#ifndef _SCCB_H
#define _SCCB_H

#define SCCB_SIC_H()     GPIO_PORTB_DATA_R |= 0x00000004; // PB2 = 1  
#define SCCB_SIC_L()     GPIO_PORTB_DATA_R &= ~0x00000004; // PB2 = 0 

#define SCCB_SID_H()     GPIO_PORTB_DATA_R |= 0x00000020; // PB5 = 1  
#define SCCB_SID_L()     GPIO_PORTB_DATA_R &= ~0x00000020; // PB5 = 0
/**/
#define SCCB_SID_IN      SCCB_SID_GPIO_INPUT();
#define SCCB_SID_OUT     SCCB_SID_GPIO_OUTPUT();


#define SCCB_SID_STATE	 (GPIO_PORTB_DATA_R & 0x00000020) // Read PB5

///////////////////////////////////////////	 
void SCCB_GPIO_Config(void);
void SCCB_SID_GPIO_OUTPUT(void);
void SCCB_SID_GPIO_INPUT(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
unsigned char SCCBwriteByte(unsigned char m_data);
unsigned char SCCBreadByte(void);


#endif /* _SCCB_H */


