#include "rpi.h"
#include "bit-support.h"

void caches_enable(void) {
    unsigned r;
    asm volatile ("MRC p15, 0, %0, c1, c0, 0" : "=r" (r));
	r |= (1 << 12); // l1 instruction cache
	r |= (1 << 11); // branch prediction
    asm volatile ("MCR p15, 0, %0, c1, c0, 0" :: "r" (r));
}

// should we flush icache?
void caches_disable(void) {
    unsigned r;
    asm volatile ("MRC p15, 0, %0, c1, c0, 0" : "=r" (r));
    //r |= 0x1800;
	r &= ~(1 << 12); // l1 instruction cache
	r &= ~(1 << 11); // branch prediction
    asm volatile ("MCR p15, 0, %0, c1, c0, 0" :: "r" (r));
}

int caches_is_enabled(void) {
    unsigned r;
    asm volatile ("MRC p15, 0, %0, c1, c0, 0" : "=r" (r));

    return bit_get(r, 12) && bit_get(r,11);
}
