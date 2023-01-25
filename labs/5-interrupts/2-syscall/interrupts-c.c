#include "rpi.h"
#include "rpi-interrupts.h"

// initialize global interrupt state.
void int_init_vec(uint32_t *vec, uint32_t *vec_end) {
    // put interrupt flags in known state. 
    //  BCM2835 manual, section 7.5
    PUT32(Disable_IRQs_1, 0xffffffff);
    PUT32(Disable_IRQs_2, 0xffffffff);
    dev_barrier();

    /*
     * Copy in interrupt vector table and FIQ handler _table and _table_end
     * are symbols defined in the interrupt assembly file, at the beginning
     * and end of the table and its embedded constants.
     */

    // where the interrupt handlers go.
#   define RPI_VECTOR_START  0
    uint32_t *dst = (void*)RPI_VECTOR_START,
                 *src = vec,
                 n = vec_end - src;
    for(int i = 0; i < n; i++)
        dst[i] = src[i];
}
void int_init(void) {
    extern uint32_t _interrupt_table[];
    extern uint32_t _interrupt_table_end[];
    int_init_vec(_interrupt_table, _interrupt_table_end);
}

#define UNHANDLED(msg,r) \
	panic("ERROR: unhandled exception <%s> at PC=%x\n", msg,r)
void fast_interrupt_vector(unsigned pc) {
	UNHANDLED("fast", pc);
}

// this is used for syscalls.
void software_interrupt_vector(unsigned pc) {
	UNHANDLED("soft interrupt", pc);
}
void reset_vector(unsigned pc) {
	UNHANDLED("reset vector", pc);
}
void undefined_instruction_vector(unsigned pc) {
	UNHANDLED("undefined instruction", pc);
}
void prefetch_abort_vector(unsigned pc) {
	UNHANDLED("prefetch abort", pc);
}
void data_abort_vector(unsigned pc) {
	UNHANDLED("data abort", pc);
}
