// simple breakpoint test:
//  1. set a single breakpoint on <foo>.
//  2. in exception handler, make sure it's a debug exception and disable.
//  3. if that worked, do 1&2 <n> times to make sure works.
#include "rpi.h"
#include "debug-fault.h"

// the routine we fault on: we don't want to use GET32 or PUT32
// since that would break printing.
void foo(int x);

// total number of faults.
static volatile int n_faults = 0;

static void brkpt_handler(uint32_t lr, uint32_t regs[16]) {
    assert(fault_is_brkpt());

    // 13-34: effect of exception on watchpoint registers.
    output("lr=%x, foo=%x\n", lr, foo);
    assert(lr == (uint32_t)foo);

    // increment fault count, disable the fault and jump back.
    n_faults++;

    brkpt_disable0();
    assert(!brkpt_is_enabled0());
}

void notmain(void) {
    debug_fault_init();

    brkpt_set0((uint32_t)foo, brkpt_handler);
    output("set breakpoint for addr %p\n", foo);


    output("about to call %p: should see a fault!\n", foo);
    assert(brkpt_is_enabled0());
    foo(0);
    assert(!brkpt_is_enabled0());

    assert(n_faults == 1);

    int n = 10;
    trace("worked!  fill do %d repeats\n", n);
    for(int i = n_faults = 0; i < n; i++) {
        brkpt_enable0();
        assert(brkpt_is_enabled0());

        trace("should see a breakpoint fault!\n");
        assert(brkpt_is_enabled0());
        foo(i);
        assert(!brkpt_is_enabled0());
        trace("n_faults=%d, i=%d\n", n_faults,i);
        assert(n_faults == i+1);
    }
    trace("SUCCESS\n");
}

// weak attempt at preventing inlining.
void foo(int x) {
    trace("running foo: %d\n", x);
}
