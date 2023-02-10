#include "rpi.h"
#include "vector-base.h"

#include "debug-fault.h"

// currently only handle a single breakpoint.
static bfault_handler_t brkpt_handler = 0;
// currently only handle a single watchpoint
static wfault_handler_t watchpt_handler = 0;

// if we have a breakpoint fault, call the breakpoint
// handler.
void prefetch_abort_vector(unsigned lr) {
    // lr needs to get adjusted for watchpoint fault?
    // we should be able to return lr?
    if(was_brkpt_fault())
        brkpt_handler(lr,0);
    else 
        panic("should not get another fault\n");
}

// if we have a dataabort fault, call the watchpoint
// handler.
void data_abort_vector(unsigned lr) {
    // watchpt handler.
    if(was_brkpt_fault())
        panic("should only get debug faults!\n");
    if(!was_watchpt_fault())
        panic("should only get watchpoint faults!\n");

    assert(watchpt_handler);

    // instruction that caused watchpoint.
    uint32_t pc = watchpt_fault_pc();
    // addr that the load or store was on.
    void *addr = (void*)watchpt_fault_addr();
    watchpt_handler(addr, pc, 0); 
}

// setup handlers: enable cp14
void debug_fault_init(void) {
    unimplemented();

    // just started, should not be enabled.
    assert(!cp14_bcr0_is_enabled());
    assert(!cp14_bcr0_is_enabled());
}

// set a breakpoint on <addr>: call <h> when triggers.
void brkpt_set0(uint32_t addr, bfault_handler_t h) {
    unimplemented();

    assert(cp14_bcr0_is_enabled());
    brkpt_handler = h;
}

// set a watchpoint on <addr>: call handler <h> when triggers.
void watchpt_set0(uint32_t addr, wfault_handler_t h) {
    unimplemented();


    assert(cp14_wcr0_is_enabled());
    watchpt_handler = h;
}
