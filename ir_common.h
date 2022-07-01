#ifndef IR_COMMON_H_INCLUDED
#define IR_COMMON_H_INCLUDED

#define _BV(n) (1<<(n))

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) ( sfr &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (sfr |= _BV(bit))
#endif


#endif // IR_COMMON_H_INCLUDED
