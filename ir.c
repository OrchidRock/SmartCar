#include "ir_remote.h"
#include "ir.h"


decode_results results; // container for received IR codes

void IRremote_Init(){
        // PORTB pin 0
        irrecv_set_pin(); // PB0	
        ir_blink(1);
        ir_enable_in(4000); // Start the receiver/ tick 50 us
}
void beginIRremote(){
}

int IRrecived(){
        return irrecv_decode(&results);
}

void resumeIRremote(){
        ir_resume(); // resume receiver
}

unsigned long getIRresult(){
        return results.value;
}
