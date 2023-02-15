/* 
   reg hash = 0xcd6e5626
 */
#include "rpi.h"
#include "asm-helpers.h"
#include "cpsr-util.h"
#include "breakpoint.h"
#include "vector-base.h"
#include "fast-hash32.h"

void switchto_user_asm(uint32_t regs[16]);
void mov_ident(void);

static uint32_t reg_hash;
static uint32_t inst_cnt;

static void dump_regs(uint32_t regs[17]) {
    for(unsigned i = 0; i < 17; i++)
        if(regs[i])
            trace("reg[%d]=%x\n", i, regs[i]);
}

int do_syscall(uint32_t regs[17]) {
    int sysno = regs[0];
    trace("------------------------------------------------\n");
    trace("in syscall: sysno=%d\n", sysno);
    dump_regs(regs);
    assert(sysno == 0);
    trace("FINAL: total instructions=%d, reg-hash=%x\n", inst_cnt, reg_hash);
    clean_reboot();
}

void single_step_full(uint32_t regs[17]) {
    uint32_t pc = regs[15];

    if(!brkpt_fault_p())
        panic("pc=%x: is not a breakpoint fault??\n", pc);

    trace("------------------------------------------------\n");
    reg_hash = fast_hash_inc32(regs, 17*sizeof regs[0], reg_hash);
    trace("cnt=%d: single step pc=%x, hash=%x\n", inst_cnt++, pc, reg_hash);
    dump_regs(regs);

    uint32_t spsr = spsr_get();
    if(mode_get(spsr) != USER_MODE)
        panic("pc=%x: is not at user level: <%s>?\n", pc, mode_str(spsr));
    if(regs[16] != spsr)
        panic("saved spsr %x not equal to <%x>?\n", regs[16], spsr);

    brkpt_mismatch_set(pc);
    switchto_user_asm(regs);
}

void notmain(void) {
    extern uint32_t test5_full_single[];
    vector_base_set(test5_full_single);
    brkpt_mismatch_start(); 

    output("about to check that swi test works\n");
    uint32_t regs[17];
    memset(regs, 0, sizeof regs);
    regs[15] = (uint32_t)mov_ident;   // in <start.S>
    regs[16] = USER_MODE;
    brkpt_mismatch_set(regs[15]);
    switchto_user_asm(regs);
}
