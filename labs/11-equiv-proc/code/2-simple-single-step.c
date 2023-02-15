/* 
   make sure single step works.  should see:
        TRACE:simple_single_step:single step pc=0x8044
        TRACE:simple_single_step:single step pc=0x8048
        TRACE:simple_single_step:single step pc=0x804c
        TRACE:simple_single_step:single step pc=0x8050
        TRACE:simple_single_step:single step pc=0x8054
        TRACE:simple_single_step:single step pc=0x8058
        TRACE:simple_single_step:single step pc=0x805c
        TRACE:simple_single_step:single step pc=0x8060
        TRACE:simple_single_step:single step pc=0x8064
        TRACE:simple_single_step:single step pc=0x8068
        TRACE:simple_single_step:single step pc=0x806c
        TRACE:simple_single_step:single step pc=0x8070
 */
#include "rpi.h"
#include "asm-helpers.h"
#include "cpsr-util.h"
#include "breakpoint.h"
#include "vector-base.h"

void do_syscall(uint32_t sysno) {
    trace("in syscall: sysno=%d\n", sysno);
    assert(sysno == 0);
    clean_reboot();
}

void simple_single_step(uint32_t pc) {
    if(!brkpt_fault_p())
        panic("pc=%x: is not a breakpoint fault??\n", pc);

    uint32_t spsr = spsr_get();
    if(mode_get(spsr) != USER_MODE)
        panic("pc=%x: is not at user level: <%s>?\n", pc, mode_str(spsr));

    trace("single step pc=%x\n", pc);
    brkpt_mismatch_set(pc);
}

void rfe_asm(uint32_t regs[2]);
void nop_10(void);

void notmain(void) {
    extern uint32_t simple_single_except[];
    vector_base_set(simple_single_except);
    brkpt_mismatch_start(); 

    // from <1-srs-rfe.c>
    uint32_t regs[2];
    regs[0] = (uint32_t)nop_10;   // in <1-srs-rfe-asm.S>
    regs[1] = USER_MODE;
    trace("about to jump to pc=[%x] with cpsr=%x\n",
            regs[0], regs[1]);
    rfe_asm(regs);
}
