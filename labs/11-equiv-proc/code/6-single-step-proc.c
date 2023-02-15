/* 
 */
#include "rpi.h"
#include "asm-helpers.h"
#include "cpsr-util.h"
#include "breakpoint.h"
#include "vector-base.h"
#include "fast-hash32.h"

#include "proc.h"

void mov_ident(void);

static int verbose_p;
static void dump_regs(uint32_t regs[17]) {
    if(!verbose_p)
        return;

    for(unsigned i = 0; i < 17; i++)
        if(regs[i])
            output("reg[%d]=%x\n", i, regs[i]);
}

int do_syscall(uint32_t regs[17]) {
    int sysno = regs[0];
    assert(sysno == 0);

    if(verbose_p) {
        output("------------------------------------------------\n");
        output("in syscall: sysno=%d\n", sysno);
        dump_regs(regs);
    }
    proc_t *p = curproc_get();
    trace("FINAL: pid=%d: total instructions=%d, reg-hash=%x\n", 
            p->pid,
            p->inst_cnt, 
            p->reg_hash);

    if(p->expected_hash) {
        if(p->expected_hash != p->reg_hash) {
            panic("hash mismatch: expected %x, have %x\n", 
                p->expected_hash, p->reg_hash);
        }
    }

    proc_exit();
    proc_run_one();
    not_reached();
}

void single_step_full(uint32_t regs[17]) {
    uint32_t pc = regs[15];

    if(!brkpt_fault_p())
        panic("pc=%x: is not a breakpoint fault??\n", pc);

    proc_t *p = curproc_get();
    todo("compute reg hash and instructions.\n");

    if(verbose_p)  {
        output("------------------------------------------------\n");
        output("cnt=%d: single step pc=%x, hash=%x\n", p->inst_cnt, pc, p->reg_hash);
        dump_regs(regs);
    }

    uint32_t spsr = spsr_get();
    if(mode_get(spsr) != USER_MODE)
        panic("pc=%x: is not at user level: <%s>?\n", pc, mode_str(spsr));
    if(regs[16] != spsr)
        panic("saved spsr %x not equal to <%x>?\n", regs[16], spsr);

    
    proc_save(regs);
    proc_run_one();
}

void hello(void) {
    output("hello world from pid=%d\n", curproc_get()->pid);
    asm volatile("swi 1" ::: "memory");
}

void notmain(void) {
    kmalloc_init_set_start((void*)(1024*1024), 1024*1024);

    extern uint32_t test5_full_single[];
    vector_base_set(test5_full_single);
    brkpt_mismatch_start(); 

    void nop_1(void);

    output("about to check that swi test works\n");
    enum { N = 10 };
    for(int i = 0; i < N; i++) {
        //proc_fork_nostack(mov_ident, 0xcd6e5626);
        //proc_fork_nostack(nop_1, 0xbfde46be);
        proc_fork_stack(hello, 0);
    }
    proc_run_one();
}
