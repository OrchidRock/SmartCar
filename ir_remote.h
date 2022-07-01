#ifndef IR_REMOTE_H_INCLUDED
#define IR_REMOTE_H_INCLUDED

typedef struct {
    unsigned long value;
    int bits;
    volatile unsigned int * rawbuf;
    int rawlen;
} decode_results;


// functions
void irrecv_set_pin(void);//port:pin
void ir_blink(int);
int irrecv_decode(decode_results*);
void ir_enable_in(unsigned long);
void ir_resume(void);



#define NEC 1

// Decoded value for NEC when a repeat code is received
#define REPEAT 0xffffffff



// Some useful constants

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 100 // Length of raw duration buffer

// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
#define MARK_EXCESS 80

#endif // IR_REMOTE_H_INCLUDED
