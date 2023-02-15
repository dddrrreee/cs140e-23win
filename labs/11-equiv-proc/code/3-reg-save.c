/* 
    save all registers in ascending order.

        TRACE:do_syscall:reg[1]=0x1
        TRACE:do_syscall:reg[2]=0x2
        TRACE:do_syscall:reg[3]=0x3
        TRACE:do_syscall:reg[4]=0x4
        TRACE:do_syscall:reg[5]=0x5
        TRACE:do_syscall:reg[6]=0x6
        TRACE:do_syscall:reg[7]=0x7
        TRACE:do_syscall:reg[8]=0x8
        TRACE:do_syscall:reg[9]=0x9
        TRACE:do_syscall:reg[10]=0xa
        TRACE:do_syscall:reg[11]=0xb
        TRACE:do_syscall:reg[12]=0xc
        TRACE:do_syscall:reg[13]=0xd
        TRACE:do_syscall:reg[14]=0xe
        TRACE:do_syscall:reg[15]=0x80e4
        TRACE:do_syscall:reg[16]=0x10

 */
#include "rpi.h"
#include "asm-helpers.h"
#include "cpsr-util.h"
#include "breakpoint.h"
#include "vector-base.h"

int do_syscall(uint32_t regs[17]) {
    int sysno = regs[0];
    trace("in syscall: sysno=%d\n", sysno);

    for(unsigned i = 0; i < 17; i++)
        if(regs[i])
            trace("reg[%d]=%x\n", i, regs[i]);

    assert(sysno == 0);
    clean_reboot();
}

#if 0
void simple_single_step(uint32_t regs[17]) {
    static int cnt;

    if(!brkpt_fault_p())
        panic("pc=%x: is not a breakpoint fault??\n", pc);

    trace("---------------------------------------\n");
    trace("cnt=%d: single step pc=%x\n", i, regs[15]);
    for(unsigned i = 0; i < 17; i++)
        if(regs[i]))
            trace("reg[%d]=%x\n", i, regs[i]);

    uint32_t spsr = spsr_get();
    if(mode_get(spsr) != USER_MODE)
        panic("pc=%x: is not at user level: <%s>?\n", pc, mode_str(spsr));
    if(regs[16] != spsr)
        panic("saved spsr %x not equal to <%x>?\n", regs[16], spsr);
    brkpt_mismatch_set(pc);
}
#endif

void rfe_asm(uint32_t regs[2]);
void nop_10(void);
void mov_ident(void);

void notmain(void) {
    extern uint32_t swi_test_handlers[];
    vector_base_set(swi_test_handlers);
    // brkpt_mismatch_start(); 

    output("about to check that swi test works\n");
    // from <1-srs-rfe.c>
    uint32_t regs[0];
    regs[0] = (uint32_t)mov_ident;   // in <start.S>
    regs[1] = USER_MODE;
    trace("about to jump to pc=[%x] with cpsr=%x\n",
            regs[0], regs[1]);
    rfe_asm(regs);
}
