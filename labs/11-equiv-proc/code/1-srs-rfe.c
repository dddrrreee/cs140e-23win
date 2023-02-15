// simple example for how to:
//  -  use <srs> to save the current <lr> and <spsr> into a memory
//     location of two elements.
//  - 
#include "rpi.h"
#include "asm-helpers.h"
#include "cpsr-util.h"

// holds the registers you want to resume from.
void rfe_asm(uint32_t regs[2]);

// do an srs and write the result into <lr_spsr>.
// returns the current mode's lr so can compare.
uint32_t srs_super(uint32_t lr_spsr[2]);

#if 0
static inline uint32_t spsr_get(void) {
    uint32_t spsr;
    asm volatile("mrs %0,spsr" : "=r"(spsr));
    return spsr;
}
static inline void spsr_set(uint32_t spsr) {
    asm volatile("msr spsr,%0" :: "r"(spsr));
    prefetch_flush();
    assert(spsr == spsr_get());
}
#endif

// in <1-srs-rfe-asm.S>: calls <rfe_c_code>
void rfe_trampoline(void);

// called by <rfe_trampoline>.
//   [can extend: try to go to each different mode.]
//
// prints out the stack pointer passed in and checks that it's
// indeed running at USER_MODE.
void rfe_c_code(uint32_t sp) {
    trace("sp=[%x], should equal INT_STACK_ADDR=%x\n", 
            sp, INT_STACK_ADDR);
    assert(sp == INT_STACK_ADDR);

    uint32_t cpsr = cpsr_get();
    // NOTE: carry flags could be different for different compilers.
    trace("cpsr=[%x], mode=%s\n", cpsr, mode_str(cpsr));

    if(mode_get(cpsr) != USER_MODE)
        panic("wrong mode!\n");
    else
        trace("SUCCESS: we are at user level!\n");

    clean_reboot();
}

void notmain(void) {
    demand(mode_is_super(), 
            "<srs_super> won't work unless at SUPER mode");

    /***********************************************************
     * part 1: make sure that <srs> works as we expect:
     *  1. set spsr to a known value (for us: USER_MODE)
     *  2. call <srs_super> which uses <srs> at SUPER_MODE
     *     to write the SUPER lr and spsr to a passed in
     *     2 element array.  it also returns <lr> for comparison.
     */
    uint32_t lr_spsr[2] = { 0xfeedface, 0xdeadbeef };

    // set spsr to known value so we can compare.
    spsr_set(USER_MODE);

    // get <lr> and <spsr> using <srs_super> (see 1-srs-rfe-asm.S)
    uint32_t real_lr = srs_super(lr_spsr);

    // now: check that what we got is what we expect. 

    // note: you'd use <srs> in an exception handler so the 
    // the <lr> would be the <pc> we are supposed to resume to.
    trace("lr=[%x] (expect=[%x]), spsr=%x (expect=%x)\n", 
            lr_spsr[0], 
            real_lr,
            lr_spsr[1],
            spsr_get());

    // check that what <srs> wrote is the same as the
    // real lr returned by <srs_super>
    assert(lr_spsr[0]  == real_lr);

    // the <spsr> written by <srs> should be the same
    // as what we previously set above.
    assert(lr_spsr[1]  == USER_MODE);
    // double check that the <spsr> is still what we set it to.
    assert(spsr_get() == USER_MODE);
    // triple
    assert(spsr_get() == USER_MODE);
    trace("SUCCESS: <srs> matched what we expected\n");

    /***********************************************************
     * part 2: make sure rfe works as expected.
     *   - set the pc entry to <rfe_trampoline>
     *   - set the spsr entry to <USER_MODE>
     *   - run and check that it indeed switched and called 
     *     the right place. 
     */

    // do a simple <rfe> that will call <rfe_trampoline>
    // with <cpsr>=USER_MODE
    //      see: <1-srs-rfe-asm.S>
    uint32_t regs[2];
    regs[0] = (uint32_t)rfe_trampoline;   // in <1-srs-rfe-asm.S>
    regs[1] = USER_MODE;
    trace("about to jump to pc=[%x] with cpsr=%x\n",
            regs[0], regs[1]);
    rfe_asm(regs);
    not_reached();
}
