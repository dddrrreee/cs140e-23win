/* 
    save/restore all registers in ascending order.
    
    should get same result as test 3.

        TRACE:do_syscall:in syscall: sysno=0
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

void switchto_user_asm(uint32_t regs[16]);
void mov_ident(void);

void notmain(void) {
    extern uint32_t test_restore_handlers[];
    vector_base_set(test_restore_handlers);
    // brkpt_mismatch_start(); 

    output("about to check that swi test works\n");
    // from <1-srs-rfe.c>
    uint32_t regs[17];
    memset(regs, 0, sizeof regs);

    regs[15] = (uint32_t)mov_ident;   // in <start.S>
    regs[16] = USER_MODE;
    switchto_user_asm(regs);
}
