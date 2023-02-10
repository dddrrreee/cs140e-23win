// should have the same output as 1-brkpt-test.c

// simple breakpoint test:
//  1. set a single breakpoint on <foo>.
//  2. in exception handler, make sure it's a debug exception and disable.
//  3. if that worked, do 1&2 <n> times to make sure works.
#include "rpi.h"
#include "vector-base.h"
#include "armv6-debug-impl.h"

// the routine we fault on: we don't want to use GET32 or PUT32
// since that would break printing.
void foo(int x);

// total number of faults.
static volatile int n_faults = 0;

/*
   13-11:
        RULE: "must first check IFSR or DFSR to determine a debug
        exception has occured before checking `DSCR`).

   See:
     - IFSR 3-67 in docs/arm1176-fault-regs.pdf
     - DSCR 13-35 in docs/arm1176-ch13-debug.pdf 

  - `IFSR`: set whenever breakpoint occurs.   Checked in prefetch abort.

  - 13-35:  on breakpoint:
      - Check `DSCR[5:2]`,
      - IFSR set as table 13-23 describes.  Handler must check this to see if
        its a breakpoint or normal prefetch abort.
      - hardware sets up the normal prefetch abort state.
      - Instruction causing the fault is in `r14` plus 4.
*/
void prefetch_abort_vector(unsigned lr) {
    // lr needs to get adjusted for watchpoint fault?
    // we should be able to return lr?
    if(!was_brkpt_fault())
        panic("should only get a breakpoint fault\n");

    // 13-34: effect of exception on watchpoint registers.
    output("lr=%x, foo=%x\n", lr, foo);
    assert(lr == (uint32_t)foo);

    output("ifar=%x\n", cp15_ifar_get());
    output("ifsr=%x\n", cp15_ifsr_get());
    output("dscr[2:5]=%b\n", bits_get(cp14_dscr_get(), 2, 5));

    // increment fault count, disable the fault and jump back.
    n_faults++;
    cp14_bcr0_disable();
    assert(!cp14_bcr0_is_enabled());
}


void notmain(void) {
    // 1. install exception handlers: must have a valid trampoline for
    // prefetch_abort_vector
    unimplemented();

    // 2. enable the debug coprocessor.
    cp14_enable();

    // just started, should not be enabled.
    assert(!cp14_bcr0_is_enabled());


    /*
      3. set a simple breakpoint.  from 13-45:
         1. read the bcr
         2. clear enable bit: write back
         3. write the imva to BVR
         4. write BCR: BCR[22:21] = 00 or 1
            BCR[20] = 0 no linking
            BCR[15:14]: secure access as required
            BCR[8:5] base addres as required
            BCR[2:1]: super visor access as required
            BCR[0] = 1
        prefetch flush.
    */

    /* 
     * see 13-17 for how to set bits
     * set:
     *   - match
     *   - disable linking
     *   - match in secure and non-secure world
     *   - match load of any location in word
     *   - supervisor or not
     *   - enabled.
     */
    uint32_t b = 0;


    // set breakpoint using bcr0 and bvr0
    unimplemented();

    assert(cp14_bcr0_is_enabled());
    output("set breakpoint for addr %p\n", foo);

    output("about to call %p: should see a fault!\n", foo);
    foo(0);
    assert(!cp14_bcr0_is_enabled());
    assert(n_faults == 1);

    int n = 10;
    trace("worked!  fill do %d repeats\n", n);
    for(int i = n_faults = 0; i < n; i++) {
        cp14_bcr0_enable();
        assert(cp14_bcr0_is_enabled());

        trace("should see a breakpoint fault!\n");
        foo(i);
        assert(!cp14_bcr0_is_enabled());
        trace("n_faults=%d, i=%d\n", n_faults,i);
        assert(n_faults == i+1);
    }
    trace("SUCCESS\n");
}

// weak attempt at preventing inlining.
void foo(int x) {
    trace("running foo: %d\n", x);
}
