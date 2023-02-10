#ifndef __RPI_INTERRUPT_H__
#define __RPI_INTERRUPT_H__

#include "rpi.h"

// from the valvers description.

/** @brief Bits in the Enable_Basic_IRQs register to enable various interrupts.
   See the BCM2835 ARM Peripherals manual, section 7.5 */
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
#define RPI_BASIC_ARM_MAILBOX_IRQ       (1 << 1)
#define RPI_BASIC_ARM_DOORBELL_0_IRQ    (1 << 2)
#define RPI_BASIC_ARM_DOORBELL_1_IRQ    (1 << 3)
#define RPI_BASIC_GPU_0_HALTED_IRQ      (1 << 4)
#define RPI_BASIC_GPU_1_HALTED_IRQ      (1 << 5)
#define RPI_BASIC_ACCESS_ERROR_1_IRQ    (1 << 6)
#define RPI_BASIC_ACCESS_ERROR_0_IRQ    (1 << 7)

// bcm2835, p112   [starts at 0x2000b200]
enum { 
    IRQ_Base            = 0x2000b200,
    IRQ_basic_pending   = IRQ_Base+0,       // 0x200
    IRQ_pending_1       = IRQ_Base+4,       // 0x204
    IRQ_pending_2       = IRQ_Base+8,       // 0x208
    FIQ_control         = IRQ_Base+0xc,     // 0x20c
    Enable_IRQs_1       = IRQ_Base+0x10,    // 0x210
    Enable_IRQs_2       = IRQ_Base+0x14,    // 0x214
    Enable_Basic_IRQs   = IRQ_Base+0x18,    // 0x218
    Disable_IRQs_1      = IRQ_Base+0x1c,    // 0x21c
    Disable_IRQs_2      = IRQ_Base+0x20,    // 0x220
    Disable_Basic_IRQs  = IRQ_Base+0x24,    // 0x224
};

// bit-wise AND this with IRQ_basic_pending to see if it's a timer interrupt.
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)

// these can't be included on the fake-pi.  pull them out
// or switch to macros?

#if 0
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
static inline int int_is_enabled(void) {
    // 7ths bit = 1 ===> disabled.
    return ((cpsr_get() >> 7) & 1) == 0;
}
#endif

void system_enable_interrupts(void);
void system_disable_interrupts(void);
void int_init(void);
void int_init_reg(void *int_vector_addr);

// need to disable mmu if it's enabled.
#define INT_UNHANDLED(msg,r) \
    panic("called default handler: unhandled exception <%s> at PC=%x\n", msg,r)

// can override these.
void data_abort_vector(unsigned pc);
void prefetch_abort_vector(unsigned pc);
void reset_vector(unsigned pc);
void interrupt_vector(unsigned pc);
void software_interrupt_vector(unsigned pc);

// one time clearing of interrupt state.
void int_clear_state(void);

void int_init(void);
// initialize interrupts using table <vec>
void int_init_vec(uint32_t *vec, uint32_t *vec_end);

// do a hard reset of all hardware interrupt
// sources to off (BCM and system).
//
// used during initialization.
uint32_t set_all_interrupts_off(void);

void bcm_set_interrupts_off(void);

void * int_vec_reset(void *vec);
void int_vec_init(void *v);

#endif 
