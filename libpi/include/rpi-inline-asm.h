#ifndef __RPI_INLINE_ASM_H__
#define __RPI_INLINE_ASM_H__
// try to keep most of the inline assembly in this header
// so it's easy to flip to another arch or fake pi.

// get the status register.
static inline uint32_t cpsr_get(void) {
    uint32_t cpsr;
    asm volatile("mrs %0,cpsr" : "=r"(cpsr));
    return cpsr;
}
// set the status register
static inline void cpsr_set(uint32_t cpsr) {
    asm volatile("msr cpsr, %0" :: "r"(cpsr));
}

// check if interrupts are enabled.
static inline int cpsr_int_enabled(void) {
    // 7ths bit = 1 ===> disabled.
    return ((cpsr_get() >> 7) & 1) == 0;
}

/*
  returns previous cpsr so you can do recursively.
  replaces:
    system_enable_interrupts:
        mrs r0,cpsr         @ move process status register (PSR) to r0
        bic r0,r0,#(1<<7)   @ clear 7th bit.
        msr cpsr_c,r0       @ move r0 back to PSR
        bx lr               @ return.
*/
static inline uint32_t cpsr_int_enable(void) {
    uint32_t cpsr = cpsr_get();
    cpsr_set(cpsr & ~(1<<7));
    return cpsr;
}

/*
  disable system interrupts: returns previous cpsr.

  replaces:
    .globl system_disable_interrupts
    system_disable_interrupts:
        mrs r0,cpsr
        orr r0,r0,#(1<<7)          @ set 7th bit: or in 0b100 0000
        msr cpsr_c,r0
        bx lr
 */
static inline uint32_t cpsr_int_disable(void) {
    uint32_t cpsr = cpsr_get();
    cpsr_set(cpsr | (1<<7));
    return cpsr;
}

// reset cpsr to a previous state.  returns the 
// current value.
//
// the if/else is a bit clearer than doing bit
// manipulations.
//
// we don't just reset the entire cpsr in order to 
// preserve the condition code flags. 
//
// NOTE: i think there is a suffix we can give
// to the cpsr write to get around this.  check manual.
static inline uint32_t cpsr_int_reset(uint32_t cpsr) {
    if(cpsr & (1<<7))
        return cpsr_int_disable();
    else
        return cpsr_int_enable();
}
#endif
