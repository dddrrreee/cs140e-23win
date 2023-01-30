#include "rpi.h"

// roughly 2-3x the number of cyles.  dunno if we care.  can read cycle count from
// cp15 iirc.
void delay_cycles(uint32_t ticks) {
#ifndef RPI_UNIX
    while(ticks-- > 0)
        asm("add r1, r1, #0");
#else
    void nop(void);
    while(ticks-- > 0)
        nop();
#endif
}


