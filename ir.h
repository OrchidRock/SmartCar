#ifndef IR_REMOTETOOLS_H_INCLUDED
#define IR_REMOTETOOLS_H_INCLUDED
#include "ir_remote.h"
extern void IRremote_Init(void);
extern void beginIRremote(void);

extern int IRrecived(void);

extern void resumeIRremote(void);

extern unsigned long getIRresult(void);

#endif // IR_REMOTETOOLS_H_INCLUDED
