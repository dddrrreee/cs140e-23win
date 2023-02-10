#include "rpi.h"
#include "rpi-interrupts.h"
#include "vector-base.h"
#include "rpi-inline-asm.h"

void * int_vec_reset(void *vec) {
    return vector_base_reset(vec);
}

void int_vec_init(void *v) {
    // turn off system interrupts
    cpsr_int_disable();

    //  BCM2835 manual, section 7.5 , 112
    dev_barrier();
    PUT32(Disable_IRQs_1, 0xffffffff);
    PUT32(Disable_IRQs_2, 0xffffffff);
    dev_barrier();

    vector_base_set(v);
}
