#include "rpi.h"
#include "asm-helpers.h"
#include "cpsr-util.h"
#include "breakpoint.h"
#include "vector-base.h"
#include "fast-hash32.h"

#include "proc.h"
#include "syscalls.h"
// return the procmap?
#include "pinned-vm.h"
#include "mmu.h"

#include "equiv-os.h"

// pointer to the current context.
pctx_t cur_ctx;

// configuration options.
struct config config = {0};
int do_random_enq(void) {
    return config.do_random_enq != 0;
}

// returns 1 if should print out the stats.
static inline int should_print(pctx_t *ctx) {
    uint32_t last = ctx->last_print;
    uint32_t now = timer_get_usec();

    if((now - last) > ctx->print_timeout) {
        ctx->last_print = now;
        return 1;
    }
    return 0;
}

void dump_regs(pctx_t *ctx, uint32_t regs[17]) {
    for(unsigned i = 0; i < 17; i++)
        if(regs[i])
            output("reg[%d]=%x\n", i, regs[i]);
}

// system call handler:
//   reg[0] - holds syscall number.
//   reg[1] - 1st arg
//   reg[2] - 2nd arg
//   reg[3] - 3rd arg
//
// to return a result: 
//   assign it in regs[0].
int syscall_full(uint32_t regs[17]) {
    int sysno = regs[0];
    pctx_t *ctx = ctx_get();
    proc_t *p = cur_proc_get(ctx);

    // should do every N seconds.
    if(should_print(ctx))
        pstats_print("incremental", &ctx->stats, 0);

    switch(sysno) {
        // increment the brk() value: could easily have this at 
        // user level.
        case SYS_SBRK: 
        {
            // not sure what we really should do here?
            long incr = regs[1];

            // don't panic: return error.
            if(incr > 1024*1024)
                panic("too large increment: %d\n", incr);
            output("incr=%d\n", incr);
            p->brk_addr += incr;
            char *sp = (void*)regs[13];
            if(p->brk_addr > sp)
                panic("brk corruption increment: %x, sp=%x\n", 
                    p->brk_addr, sp);
            regs[0] = (uint32_t)p->brk_addr;
            break;
        }

        // fork the current process.
        case SYS_FORK: 
            output("fork!\n");

            // need the pid of the kid.
            regs[0] = sys_fork(ctx, regs);
            break;

        // wait for the pid in regs[1]
        case SYS_WAITPID: 
            output("waitpid!\n");

            unsigned rel_pid = regs[1];

            // right now can't wait on parent.
            if(rel_pid == 0)
                todo("add code so can wait on parent\n");

            if(rel_pid > p->nkids)
                todo("add error handling for illegal pid=%d (max=%d)\n",
                    rel_pid, p->nkids);

            proc_t *k = p->kid_list[rel_pid];
            if(k->status == PROC_EXITED) {
                regs[0] = k->exit_code;
                output("waitpid: no-wait: kid=%d, exitcode=%d\n", 
                        rel_pid, regs[0]);
            } else {
                output("waitpid: must wait: kid=%d\n", 
                        rel_pid);
                // ah.  you can't check unless it goes exit.
                predicate_wait(p, pred_waitpid, k);    
                assert(p->status == PROC_BLOCKED);
                proc_save(ctx, regs);
                proc_run_one(ctx);
                not_reached();
            }
            break;
             
        // exit the current process.
        case SYS_EXIT: 
            if(ctx->verbose_p || config.emit_exit_p) {
                output("------------------------------------------------\n");
                output("in EXIT syscall: pid=%d: sysno=%d, hash=%x\n", p->pid, sysno, p->reg_hash);
                if(ctx->verbose_p) dump_regs(ctx,regs);
            }
            p->exit_code = regs[1];
            proc_exit(ctx,p);
            proc_run_one(ctx);
            break;

        // print a single character.
        case SYS_PUTC:
            rpi_putchar(regs[1]);
            regs[0] = 0;
            break;

        // print the actual pid, do it here so don't mess up
        // hash.
        case SYS_PUT_PID:
            printk("%d", p->pid);
            regs[0] = 0;
            break;

        // print a hex number.
        case SYS_PUT_HEX:
            printk("%x", regs[1]);
            regs[0] = 0;
            break;

        // print an integer.
        case SYS_PUT_INT:
            printk("%d", regs[1]);
            regs[0] = 0;
            break;
        default: panic("invalid system call\n", sysno);
    }

    // resume the current process.
    brkpt_mismatch_set(regs[15]);
    switchto_user_asm(regs);
    not_reached();
}

#if 0
#include "armv6-debug-impl.h"
#endif
// b4-20
enum {
    SECTION_XLATE_FAULT = 0b00101,
    SECTION_PERM_FAULT = 0b1101,
};

void data_abort_full(uint32_t regs[17]) {
    todo("you have to implement\n");
#if 0
    // b4-43
    unsigned dfsr = cp15_dfsr_get();
    unsigned reason = ( (dfsr >> 4) & (1<<4)) |  (dfsr & 0b1111);

    reason = bits_get(dfsr, 0, 3) | bit_get(dfsr, 10) << 10;

    // b4-44
    unsigned fault_addr;
    // get rid of these inline asm stmts.  ugh.
    asm volatile("MRC p15, 0, %0, c6, c0, 0" : "=r" (fault_addr));

    pctx_t *ctx = ctx_get();
    proc_t *p = cur_proc_get(ctx);

    switch(reason) {
    // b4-20
    case SECTION_XLATE_FAULT:
        output("all regs\n");
        dump_regs(ctx, regs);
        panic("pid=%d,asid=%d,pc=%x: section xlate fault: addr=%x\n", 
                p->pid, 
                p->asid, 
                regs[15],
                fault_addr);
    case SECTION_PERM_FAULT:
        panic("section permission fault: %x", fault_addr);
    default:
        panic("unknown reason %b\n", reason);
    }
#endif
    not_reached();
}

static void prefetch_fault(uint32_t regs[17]) {
    todo("you have to implement\n");
#if 0
    unsigned ifsr = cp15_ifsr_get();
    unsigned reason = bits_get(ifsr, 0, 3) | bit_get(ifsr, 10) << 10;
    uint32_t ifar = cp15_ifar_get();

    switch(reason) {
    case SECTION_XLATE_FAULT:
        panic("ERROR: attempting to run unmapped addr <%p> (reason=%b)\n", ifar, reason);
    case SECTION_PERM_FAULT:
        panic("ERROR: section permission fault: attempting to run addr <%p> with no permissions (reason=%b)\n", ifar, reason);
        break;
    default:
        panic("unexpected reason %b\n", reason);
    }
#endif
    not_reached();
}

void prefetch_abort_full(uint32_t regs[17]) {
    uint32_t pc = regs[15];

#if 0
    you should implement this.
    if(!brkpt_fault_p()) {
        prefetch_fault(regs);
        panic("pc=%x: is not a breakpoint fault??\n", pc);
    }
#endif
    pctx_t *ctx = ctx_get();
    proc_t *p = cur_proc_get(ctx);
    pstats_inst_inc(&ctx->stats, p);

    if(!p)
        panic("curproc null: pc=%x\n", regs[15]);

    p->reg_hash = fast_hash_inc32(regs, 17*sizeof regs[0], p->reg_hash);
    p->inst_cnt++;
    if(ctx->verbose_p)  {
        output("------------------------------------------------\n");
        output("cnt=%d: pid=%d: single step pc=%x, hash=%x\n", p->inst_cnt, p->pid, pc, p->reg_hash);
        dump_regs(ctx,regs);
    }

    uint32_t spsr = spsr_get();
    if(mode_get(spsr) != USER_MODE)
        panic("pc=%x: is not at user level: <%s>?\n", pc, mode_str(spsr));
    if(regs[16] != spsr)
        panic("saved spsr %x not equal to <%x>?\n", regs[16], spsr);

    // turn vm off and on: should get the same hashes.
    if(config.do_vm_off_on) {
        uint32_t cp15_ctrl_reg1_get(void);

        pstats_vm_off_on_inc(&ctx->stats, p); 
        uint32_t old = cp15_ctrl_reg1_get();
        staff_mmu_disable();
        assert(!mmu_is_enabled());
        staff_mmu_enable();
        assert(mmu_is_enabled());
        uint32_t new = cp15_ctrl_reg1_get();
        assert(old == new);
    }

    // default: switch every time.  if randomswitch we
    // flip a pseudo-random coin and switch roughly 1/n times.
    if(config.random_switch) {
        // just resume
        if(pi_random() % config.random_switch != 0) {
            brkpt_mismatch_set(regs[15]);
            switchto_user_asm(regs);
            not_reached();
        }
    }
    
    assert(p == ctx->cur_proc);
    proc_save(ctx, regs);
    proc_run_one(ctx);
}

#include "pinned-vm.h"
#include "mmu.h"

void notmain(void) {
    // need to install this and setup but it's based on the 
    // configuration options.
    //
    // easiest if you can do back to back runs from scratch?
    extern uint32_t  equiv_full_vec[];
    vector_base_set(equiv_full_vec);

    if(config.do_caches_p) {
        todo("you'll need to figure out how to flush things\n");
        caches_enable();
        if(!caches_is_enabled())
            panic("cache not enabled??\n");
    }

    memset(&cur_ctx, 0, sizeof cur_ctx);
    cur_ctx.verbose_p = 0;
    cur_ctx.last_print = timer_get_usec();
    cur_ctx.print_timeout = 1000*1000*5;

    equiv_driver(&cur_ctx);
}
