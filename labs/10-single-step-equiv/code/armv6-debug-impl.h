#ifndef __ARMV6_DEBUG_IMPL_H__
#define __ARMV6_DEBUG_IMPL_H__
#include "armv6-debug.h"

// all your code should go here.  implementation of the debug interface.

// example of how to define get and set for status registers
coproc_mk(status, p14, 0, c0, c1, 0)

// you'll need to define these and a bunch of other routines.
static inline uint32_t cp15_dfsr_get(void);
static inline uint32_t cp15_ifar_get(void);
static inline uint32_t cp15_ifsr_get(void);
static inline uint32_t cp14_dscr_get(void);

static inline uint32_t cp14_wcr0_set(uint32_t r);

// return 1 if enabled, 0 otherwise.  
//    - we wind up reading the status register a bunch:
//      could return its value instead of 1 (since is 
//      non-zero).
static inline int cp14_is_enabled(void) {
    uint32_t reg;
    asm volatile("mrc p14, 0, %0, c1, c0, 0" : "=r" (reg));
    return (reg & (1 << 15)) && !(reg & (1 << 14));
    //unimplemented();
}

// enable debug coprocessor 
static inline void cp14_enable(void) {
    // if it's already enabled, just return?
    if(cp14_is_enabled())
        panic("already enabled\n");

    uint32_t val = 0;
    MRC(p14, 0, val, c1, c0, 0);
    val |= 0x8000;
    val &= ~0x4000;
    MCR(p14, 0, val, c1, c0, 0);

    // uint32_t reg;
    // asm volatile("mrc p14, 0, %0, c1, c0, 0" : "=r" (reg));
    // reg |= (1 << 15);
    // reg &= ~(1 << 14);
    // asm volatile("mcr p14, 0, %0, c1, c0, 0" :: "r" (reg));

    // for the core to take a debug exception, monitor debug mode has to be both 
    // selected and enabled --- bit 14 clear and bit 15 set.
    //unimplemented();

    assert(cp14_is_enabled());
}

// disable debug coprocessor
static inline void cp14_disable(void) {
    if(!cp14_is_enabled())
        return;
    uint32_t val = 0;
    MRC(p14, 0, val, c1, c0, 0);
    val &= ~0x8000;
    MCR(p14, 0, val, c1, c0, 0);
    // uint32_t reg;
    // asm volatile("mrc p14, 0, %0, c1, c0, 0" : "=r" (reg));
    // reg &= ~(1 << 15);
    // asm volatile("mcr p14, 0, %0, c1, c0, 0" :: "r" (reg));

    //unimplemented();

    assert(!cp14_is_enabled());
}


static inline int cp14_bcr0_is_enabled(void) {
    uint32_t val = 0;
    MRC(p14, 0, val, c0, c1, 0);
    return val & 0x1;
    //unimplemented();
}
static inline void cp14_bcr0_enable(void) {
    uint32_t val = 0;
    MRC(p14, 0, val, c0, c1, 0);
    val |= 0x1;
    MCR(p14, 0, val, c0, c1, 0);
    //unimplemented();
}
static inline void cp14_bcr0_disable(void) {
    uint32_t val = 0;
    MRC(p14, 0, val, c0, c1, 0);
    val &= ~0x1;
    MCR(p14, 0, val, c0, c1, 0);
    //unimplemented();
}

// was this a brkpt fault?
static inline int was_brkpt_fault(void) {
    uint32_t ifsr = cp15_ifsr_get();
    if ((ifsr & 0x3F) == 0x0F) {
        uint32_t dscr = cp14_dscr_get();
        if ((dscr & 0xF) == 0x0) {
            return 1;
        }
        }

    // use IFSR and then DSCR
   // unimplemented();

    return 0;
}

// was watchpoint debug fault caused by a load?
static inline int datafault_from_ld(void) {
    return bit_isset(cp15_dfsr_get(), 11) == 0;
}
// ...  by a store?
static inline int datafault_from_st(void) {
    return !datafault_from_ld();
}


// 13-33: tabl 13-23
static inline int was_watchpt_fault(void) {
    uint32_t dfsr, dscr;
    __asm__ volatile("mrc p15, 0, %0, c5, c0, 0" : "=r" (dfsr));
    __asm__ volatile("mrc p15, 0, %0, c5, c0, 2" : "=r" (dscr));
    return ((dfsr & (1 << 10)) && (dscr & 1));
    // use DFSR then DSCR
    //unimplemented();
}

static inline int cp14_wcr0_is_enabled(void) {
    uint32_t value;
    __asm__ volatile("mrc p14, 0, %0, c0, c0, 0" : "=r" (value));
    return (value & (1 << 0));
    //unimplemented();
}
static inline void cp14_wcr0_enable(void) {
    uint32_t value;
    __asm__ volatile("mrc p14, 0, %0, c0, c0, 0" : "=r" (value));
    value |= (1 << 0);
    __asm__ volatile("mcr p14, 0, %0, c0, c0, 0" : : "r" (value));
    //unimplemented();
}
static inline void cp14_wcr0_disable(void) {
    uint32_t value;
    __asm__ volatile("mrc p14, 0, %0, c0, c0, 0" : "=r" (value));
    value &= ~(1 << 0);
    __asm__ volatile("mcr p14, 0, %0, c0, c0, 0" : : "r" (value));
    //unimplemented();
}

// Get watchpoint fault using WFAR
static inline uint32_t watchpt_fault_pc(void) {
    uint32_t wfar;
    asm volatile ("mrc p14, 0, %0, c6, c0, 0" : "=r" (wfar));
    return wfar;
    //unimplemented();
}
    
#endif
