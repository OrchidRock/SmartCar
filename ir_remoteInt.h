#ifndef IR_REMOTEINT_H_INCLUDED
#define IR_REMOTEINT_H_INCLUDED

#include "ir_common.h"
#include "ir_remote.h"

#include "tm4c123gh6pm.h"

#define ERR 0
#define DECODE 1




// Pulse parms are *50-100 for the Mark and *50+100 for the space
// First MARK is the one after the long gap
// pulse parameters in usec
#define NEC_HDR_MARK	9000 // 9ms
#define NEC_HDR_SPACE	4500 // 4.5ms
#define NEC_BIT_MARK	560  // 
#define NEC_ONE_SPACE	1600 // 2250 - 560
#define NEC_ZERO_SPACE	560  // 1120 - 560
#define NEC_RPT_SPACE	2250 // 

#define TOLERANCE 25  // percent tolerance in measurements
#define LTOL (1.0 - TOLERANCE/100.)
#define UTOL (1.0 + TOLERANCE/100.)

#define _GAP 5000 // Minimum gap between transmissions
#define GAP_TICKS (_GAP/USECPERTICK)

#define TICKS_LOW(us) (int) (((us)*LTOL/USECPERTICK))
#define TICKS_HIGH(us) (int) (((us)*UTOL/USECPERTICK + 1))

#define PORT_PIN_DATA        (GPIO_PORTB_DATA_R & 0x20) // PB5
#define BLINK_UP()			 (GPIO_PORTB_DATA_R |= 0x01) // PB0 on
#define BLINK_OFF()          (GPIO_PORTB_DATA_R &= ~0x01) // PB0 off

// receiver states
#define STATE_IDLE     2
#define STATE_MARK     3
#define STATE_SPACE    4
#define STATE_STOP     5

// information for the interrupt handler
typedef struct {
  int rcvstate;          // state machine
  int blinkflag;         // TRUE to enable blinking of pin 13 on IR processing
  unsigned int timer;     // state timer, counts 50uS ticks.
  unsigned int rawbuf[RAWBUF]; // raw data
  int rawlen;         // counter of entries in rawbuf
}
irparams_t;

// Defined in ir_remote.c
extern volatile irparams_t irparams;

// IR detector output is active low
#define MARK  0
#define SPACE 1

#define TOPBIT 0x80000000

#define NEC_BITS 32

#endif // IR_REMOTEINT_H_INCLUDED
