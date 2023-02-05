#ifndef __RPI_INLINE_ASM_H__
#define __RPI_INLINE_ASM_H__

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

static inline void cpsr_int_reset(uint32_t cpsr) {
    cpsr_set(cpsr);
}

#endif
