
#include "ir_remote.h"
#include "ir_remoteInt.h"

volatile irparams_t irparams;

static int
MATCH(int measured, int desired) {return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);}

static int
MATCH_MARK(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us + MARK_EXCESS));}

static int
MATCH_SPACE(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us - MARK_EXCESS));}

static long
decodeNEC(decode_results *results){
        unsigned long data = 0;
        int offset = 1; // Skip first space
        int i;
        if(!MATCH_MARK(results->rawbuf[offset], NEC_HDR_MARK)){
                return ERR;
        }
        offset ++;
        // Check for repeat
        if (irparams.rawlen == 4 &&
                        MATCH_SPACE(results->rawbuf[offset], NEC_RPT_SPACE) &&
                        MATCH_MARK(results->rawbuf[offset+1], NEC_BIT_MARK)) {
                results->bits = 0;
                results->value = REPEAT;
                return DECODE;
        }
        if (irparams.rawlen < 2 * NEC_BITS + 4) {
                return ERR;
        }
        // Initial space
        if (!MATCH_SPACE(results->rawbuf[offset], NEC_HDR_SPACE)) {
                return ERR;
        }
        offset++;

        for (i = 0; i < NEC_BITS; i++) {
                if (!MATCH_MARK(results->rawbuf[offset], NEC_BIT_MARK)) {
                        return ERR;
                }
                offset++;
                if (MATCH_SPACE(results->rawbuf[offset], NEC_ONE_SPACE)) {
                        data = (data << 1) | 1;
                }
                else if (MATCH_SPACE(results->rawbuf[offset], NEC_ZERO_SPACE)) {
                        data <<= 1;
                }
                else {
                        return ERR;
                }
                offset++;
        }
        // Success
        results->bits = NEC_BITS;
        results->value = data;
        return DECODE;
}



/*
 * PB5 in 
 *  
 */
void irrecv_set_pin(){
        volatile unsigned long delay;
        irparams.blinkflag = 0;

        // PB5 in
        SYSCTL_RCGC2_R |= 0x00000002; // activate port B
        delay = SYSCTL_RCGC2_R;
        GPIO_PORTB_CR_R |= 0x20; // allow change for PB5
        GPIO_PORTB_AMSEL_R &= ~0x20;
        GPIO_PORTB_DIR_R &= ~0x20; // PB5 in
        GPIO_PORTB_AFSEL_R &= ~0x20;
        GPIO_PORTB_DEN_R |= 0x20; // enable digital I/O on PB5
        GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF0FFFFF)+ 
                0x00000000;
        while(delay--){}
}
/*
 * PB0 out for blin
 */

void ir_blink(int blinkflag){
        volatile unsigned long delay;
        irparams.blinkflag = blinkflag;
        if(blinkflag){ 
                // PB0 out
                SYSCTL_RCGC2_R |= 0x00000002; // activate port B
                delay = SYSCTL_RCGC2_R;
                GPIO_PORTB_CR_R |= 0x01; // allow change for PB0
                GPIO_PORTB_AMSEL_R &= ~0x01;
                GPIO_PORTB_DIR_R |= 0x01; // PB0 out
                GPIO_PORTB_AFSEL_R &= ~0x01;
                GPIO_PORTB_DEN_R |= 0x01; // enable digital I/O on PB0
                GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFFFFF0)+ 
                        0x00000000;
                while(delay--){}
        }
}

void ir_enable_in(unsigned long period){
        volatile unsigned long delay;
        // timer 2A
        SYSCTL_RCGCTIMER_R |= 0x04; // activate timer2

        delay = SYSCTL_RCGCTIMER_R;

        TIMER2_CTL_R &= ~0x00000001; // disable timer2A
        TIMER2_CFG_R = 0x00000000; // 32-bit mode
        TIMER2_TAMR_R = 0x00000002; // periodic mode
        TIMER2_TAILR_R = period - 1;// reload value
        TIMER2_TAPR_R = 0x0; // resolution is 12.5 ns
        TIMER2_ICR_R = 0x00000001 ; // clear timeout flag
        TIMER2_IMR_R = 0x00000001 ; // arm timeout

        NVIC_PRI5_R = (NVIC_PRI5_R & 0x00FFFFFF) | 0x80000000;
        // priority 4
        NVIC_EN0_R |= 1<<23; // enable IRQ23 in
        TIMER2_CTL_R |= 0x00000001; // enable timer2A

        // timer
        while(delay--){}
        irparams.rawlen = 0;
        irparams.timer = 0;
        irparams.rcvstate = STATE_IDLE;
}
int irrecv_decode(decode_results* results){
        results->rawbuf = irparams.rawbuf;
        results->rawlen = irparams.rawlen;
        if(irparams.rcvstate != STATE_STOP){
                return ERR;
        }
        if(decodeNEC(results)){
                return DECODE;
        }
        // Throw away and start over
        ir_resume();
        return ERR;
}
void ir_resume(){
        irparams.rawlen = 0;
        irparams.rcvstate = STATE_IDLE;
}
void Timer2A_Handler(void){
        //BLINK_UP();
        int ir_data;
        TIMER2_ICR_R = 0x00000001; //acknowledge
        ir_data = (PORT_PIN_DATA != 0x00000000)?1:0;
        irparams.timer++; // one more 50us
        if(irparams.rawlen >= RAWBUF){ // buf overflow
                irparams.rcvstate = STATE_STOP;
        }
        switch(irparams.rcvstate){
                case STATE_IDLE:
                        if(ir_data == MARK){ // Low Level: Data coming
                                if(irparams.timer < GAP_TICKS){
                                        irparams.timer = 0;
                                }else{
                                        irparams.rawlen = 0;
                                        irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                                        irparams.timer = 0;
                                        irparams.rcvstate = STATE_MARK;
                                }
                        }
                        break;
                case STATE_MARK: // timer mark
                        if(ir_data == SPACE){ // mark ended, record time
                                irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                                irparams.timer = 0;
                                irparams.rcvstate = STATE_SPACE;
                        }
                        break;
                case STATE_SPACE: // timing space
                        if(ir_data == MARK){ // space justed ended, record time
                                irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                                irparams.timer = 0;
                                irparams.rcvstate = STATE_MARK;
                        }else{ // space
                                if(irparams.timer > GAP_TICKS){ // big space
                                        irparams.rcvstate = STATE_STOP;
                                }
                        }
                        break;
                case STATE_STOP:// waiting, measuring gap
                        if(ir_data == MARK){
                                irparams.timer = 0;
                        }
                        break;
                default:
                        break;
        }

        if(irparams.blinkflag == 1){
                if(ir_data == MARK)
                        BLINK_UP();
                else
                        BLINK_OFF();
        }
}
