#ifndef __DEBUG_FAULT_H__
#define __DEBUG_FAULT_H__
#include "armv6-debug-impl.h"

// exception handler:
//   - pc is the pc that the fault occured.
//   - <regs> is unused
typedef void (*bfault_handler_t)(uint32_t pc, uint32_t regs[16]);

// same, except <addr> is the faulting location.
typedef void (*wfault_handler_t)(void *addr, uint32_t pc, uint32_t regs[16]);

void debug_fault_init(void);

/**********************************************************
 * breakpoint support
 */

// set breakpoint 0 on <addr>
void brkpt_set0(uint32_t addr, bfault_handler_t h);

// inline the rest for speed

// is breakpoint 0 enabled?
static inline int brkpt_is_enabled0(void) 
    { return cp14_bcr0_is_enabled(); }

// enable breakpoint 0
static inline void brkpt_enable0(void) 
    { unimplemented(); }

// disable breakpoint 0
static inline void brkpt_disable0(void) 
    { unimplemented(); }

// if the current fault is a breakpoint, return 1
static inline int fault_is_brkpt(void) 
    { return was_brkpt_fault(); }

/**********************************************************
 * watchpoint support
 */

// set breakpoint 0 on <addr>
void watchpt_set0(uint32_t addr, wfault_handler_t h);

// inline the rest for speed

// is watchpoint 0 enabled?
static inline int watchpt_is_enabled0(void) 
    { return cp14_wcr0_is_enabled(); }

// enable watchpoint 0
static inline void watchpt_enable0(void) 
    { unimplemented(); }

// disable watchpoint 0
static inline void watchpt_disable0(void) 
    { unimplemented(); }

// if the current fault is a watchpoint, return 1
static inline int fault_is_watchpt(void) 
    { return was_watchpt_fault(); }

// fault is from a load.
static inline int fault_is_ld(void) 
    { return datafault_from_ld(); }
#endif
