// handle a load/store to address 1 (bad_addr) --- use get8/put8
// this tests that you are doing subword matching correctly.
#include "rpi.h"
#include "debug-fault.h"

static int load_fault_n, store_fault_n;

static unsigned bad_addr;

// change to passing in the saved registers.
static void 
watchpt_handler(void *addr, uint32_t pc, uint32_t regs[16]) {
    assert(fault_is_watchpt());

    if(addr != (void*)bad_addr)
        panic("expected watchpt fault on addr %p, have %p\n", 
                        bad_addr, addr);

    if(fault_is_ld()) {
        output("load fault at pc=%x\n", pc);
        assert(pc == (uint32_t)GET8);
        load_fault_n++;
    } else {
        output("store fault at pc=%x\n", pc);
        assert(pc == (uint32_t)PUT8);
        store_fault_n++;
    }
    watchpt_disable0();
}

static void test_bad_addr(int i) {
    bad_addr = i;
    trace("about to test addr %x\n", bad_addr);

    watchpt_set0(bad_addr, watchpt_handler);

    assert(watchpt_is_enabled0());
    trace("set watchpoint for addr %p\n", bad_addr);

    trace("should see a store fault!\n");
    PUT8(bad_addr,0);

    if(!store_fault_n)
        panic("did not see a store fault\n");

    assert(!watchpt_is_enabled0());
    watchpt_enable0();
    assert(watchpt_is_enabled0());

    trace("should see a load fault!\n");
    GET8(bad_addr);
    if(!load_fault_n)
        panic("did not see a load fault\n");
}

void notmain(void) {
    debug_fault_init();

    for(int i = 0; i < 4; i++)
        test_bad_addr(i);

    trace("SUCCESS\n");
}
