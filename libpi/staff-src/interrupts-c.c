// initialize global interrupt state.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "rpi-inline-asm.h"

void bcm_set_interrupts_off(void) {
    // put interrupt flags in known state. 
    //  BCM2835 manual, section 7.5 , 112
    dev_barrier();
    PUT32(Disable_IRQs_1, 0xffffffff);
    PUT32(Disable_IRQs_2, 0xffffffff);
    dev_barrier();
}


// hard reset at the start of boot up.  returns previous
// cpsr interrupt state.
uint32_t set_all_interrupts_off(void) {
    bcm_set_interrupts_off();
    // disable the hardware interrupts.
    // should also turn the FIQ off.
    return cpsr_int_disable();
}

// this is obsolete.
void int_init(void) {
    bcm_set_interrupts_off();

    /*
     * Copy in interrupt vector table and FIQ handler _table and _table_end
     * are symbols defined in the interrupt assembly file, at the beginning
     * and end of the table and its embedded constants.
     */
    extern unsigned _interrupt_table;
    extern unsigned _interrupt_table_end;

    // where the interrupt handlers go.
#   define RPI_VECTOR_START  0
    unsigned *dst = (void*)RPI_VECTOR_START,
                 *src = &_interrupt_table,
                 n = &_interrupt_table_end - src;
    for(int i = 0; i < n; i++)
        dst[i] = src[i];
}
