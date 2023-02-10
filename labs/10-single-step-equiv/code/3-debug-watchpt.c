// handle a store to address 0 (null)
#include "rpi.h"
#include "debug-fault.h"

static int load_fault_n, store_fault_n;

// change to passing in the saved registers.
static void 
watchpt_handler(void *addr, uint32_t pc, uint32_t regs[16]) {
    assert(fault_is_watchpt());

    if(fault_is_ld()) {
        trace("load fault at pc=%x\n", pc);
        assert(pc == (uint32_t)GET32);
        load_fault_n++;
    } else {
        trace("store fault at pc=%x\n", pc);
        assert(pc == (uint32_t)PUT32);
        store_fault_n++;
    }
    watchpt_disable0();
}

void notmain(void) {
    debug_fault_init();

    enum { null = 0 };
    watchpt_set0(null, watchpt_handler);

    assert(watchpt_is_enabled0());
    trace("set watchpoint for addr %p\n", null);

    trace("should see a store fault!\n");
    PUT32(null,0);

    if(!store_fault_n)
        panic("did not see a store fault\n");

    assert(!watchpt_is_enabled0());
    watchpt_enable0();
    assert(watchpt_is_enabled0());

    trace("should see a load fault!\n");
    GET32(null);
    if(!load_fault_n)
        panic("did not see a load fault\n");
    
    trace("SUCCESS\n");
}
