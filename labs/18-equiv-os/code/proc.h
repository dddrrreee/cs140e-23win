#ifndef __PROC_H__
#define __PROC_H__

/*******************************************************************
 * simple process support.
 */

typedef struct proc proc_t;
#include "pstats.h"

typedef struct pin_pt pin_pt_t;

struct prog;
void vm_respawn(proc_t *p);

typedef int (*predicate_fn)(proc_t *self, void *data);

enum { MAX_KIDS = 12 };

typedef struct proc {
    uint32_t regs[17+4];
    struct proc *next;

    volatile uint32_t pid;      // raw pid

    // if hash is known apriori this is checked at exit.
    uint32_t expected_hash;     
    // count of instructions executed.
    uint32_t inst_cnt;
    // current register hash
    uint32_t reg_hash;

    // if we should respawn the process at exit.  this makes it
    // easier to check hashes: start with expected_hash=0, record
    // on exit, then respawn to make sure you keep getting the same value.
    uint32_t respawn;

    void (*fn)(void); 
    void *sp;

    // for sbrk(): the current brk() address: initially set right
    // after the .bss segment in process.
    char *brk_addr;
    // initial value brk is set to: this field used for respawning.
    char *brk_addr_init;

    // pointer to our current page table.
    pin_pt_t *pt;

    // name of process.
    const char *name;
    // pointer to the program data [if any]
    struct prog *prog;

    // this process's address space identifier.
    unsigned asid;

    // process status: 
    // - if can run is <PROC_RUNNABLE>, 
    // - if waiting for something = <PROC_BLOCKED>, 
    // - if exited = <PROC_EXITED>
    enum { PROC_RUNNABLE = 0, PROC_EXITED = 2, PROC_BLOCKED = 3 } status;

    // list of forked children: having a fixed array vs list is a gross
    // hack, but does make lookup/management easy
    struct proc *kid_list[MAX_KIDS];
    // number of kids this process has forked (if any)
    unsigned nkids;

    // the value this process exited with.
    unsigned exit_code;

    // this is  our relative pid: which fork were we from parent if any?
    unsigned forked_cnt; 
    // pointer to proc's parent (if any): only non-null if this proc
    // was a result of sys_fork()
    struct proc *parent;

    // if PROC_BLOCKED, this is a pointer to the predicate function
    // to check to see when unblocked.
    predicate_fn pred_fn;
    void *pred_data;

} proc_t;

#define E proc_t
#include "Q.h"

static int pred_waitpid(proc_t *p, void *kid) {
    proc_t *k = kid;
    assert(k->status >= 0 && k->status < PROC_BLOCKED);
    if(k->status != PROC_EXITED) {
        output("pred_waitpid: kid=%d not exited\n", k->pid);
        return 0;
    }
    output("waitpid for parent=%d: relpid=%d succeeded!\n", 
        p->pid, k->forked_cnt);
    p->regs[0] = k->exit_code;
    return 1;
}

static inline void 
predicate_wait(proc_t *p, predicate_fn fn, void *data) {
    assert(p->status == PROC_RUNNABLE);
    p->pred_fn = fn;
    p->pred_data = data;
    p->status = PROC_BLOCKED;
}

static inline int predicate_eval(proc_t *p) {
    assert(p->status == PROC_BLOCKED);
    if(p->pred_fn(p, p->pred_data)) {
        p->status = PROC_RUNNABLE;
        return 1;
    }
    return 0;
}

static inline proc_t *
proc_pt_set(proc_t *p, pin_pt_t *pt) {
    p->pt = pt;
    return p;
}

#define curproc_get()  ({ assert(cur_proc); cur_proc; })

// very bad form: this only works if it's included a single time.
static int pid_get(void) {
    static int npid = 0;
    return ++npid;
}

struct pin_pt;
typedef struct pctx {
    Q_t runq;
    Q_t exitq;
    pstats_t stats;
    int verbose_p;

    uint32_t print_timeout;
    uint32_t last_print;

    // current running proc
    struct proc * volatile cur_proc;

    void *null_pt;
    struct pin_pt *global_pt;
} pctx_t;
extern pctx_t cur_ctx;

static inline pctx_t *ctx_get(void) {
    return &cur_ctx;
}

static inline proc_t *cur_proc_get(pctx_t *ctx) {
    assert(ctx);
    assert(ctx->cur_proc);
    return ctx->cur_proc;
}

void switchto_user_asm(uint32_t regs[17]);

#include "helper-macros.h"
#include "pi-random.h"

static inline void runq_enq(pctx_t *ctx, proc_t *p) {
    // equiv-os.h
    int do_random_enq(void);

    if(!do_random_enq())
        Q_append(&ctx->runq, p);
    else {
        if(pi_random() % 2 == 0)
            Q_append(&ctx->runq, p);
        else
            Q_push(&ctx->runq, p);
    }
}

static inline void regs_init(uint32_t regs[17], void (*fn)(void), void *sp) {
    memset(regs, 0, 17*4);
    regs[15] = (uint32_t)fn;
    regs[16] = USER_MODE;
    regs[13] = (uint32_t)sp;
    // * ah: set p->regs[14] to exit.
}

static inline void 
proc_init(proc_t *p, uint32_t expected_hash, void (*fn)(void), void *sp) {
    assert((uint32_t)p%8==0);
    assert(is_aligned_ptr(p->regs, 8));
    p->expected_hash = expected_hash;

    regs_init(p->regs, fn, sp);
    p->pid = pid_get();
    p->fn = fn;
    p->sp = sp;
    p->status = PROC_RUNNABLE;

    // right after.
    p->brk_addr =(void*) &p[1];
    /// output("PID=%d\n", p->pid);
    // assert(p->pid > 0 && p->pid < 64);
}

// can do all this with vm off, or on and aliased.

enum { STACK_SIZE = 1024*64 };

static inline proc_t * 
proc_fork_nostack(pctx_t *ctx, void (*fn)(void), uint32_t expected_hash) {
    proc_t *p = kmalloc(sizeof *p);
    proc_init(p, expected_hash, fn, 0);
    runq_enq(ctx, p);

    // is there a reason to pass in p?
    pstats_forked_inc(&ctx->stats,p);
    return p;
}

static inline proc_t * 
proc_respawn(pctx_t *ctx, proc_t *p) {
    proc_init(p, p->reg_hash, p->fn, p->sp);
    p->reg_hash = 0;
    p->pid = pid_get(); // ++npid;
    vm_respawn(p);
    runq_enq(ctx, p);
    pstats_respawn_inc(&ctx->stats, p);
    return p;
}

// do we do something else?  seems a bit not much.
//
// can pull all the other stuff in --- what should we do about
// cur proc?
static void proc_exit(pctx_t *ctx, proc_t *p) {
    if(ctx->verbose_p) 
        trace("FINAL: pid=%d: total instructions=%d, reg-hash=%x\n",
                p->pid,
                p->inst_cnt,
                p->reg_hash);

    pstats_exit_inc(&ctx->stats, p);

    if(p->expected_hash) {
        pstats_checked_inc(&ctx->stats, p);

        if(p->expected_hash != p->reg_hash) {
            panic("hash mismatch: expected %x, have %x\n",
                    p->expected_hash, p->reg_hash);
        }
    }

    // we were forked: if we were forked, and the parent has not exited,
    // add it to the parent list.
    assert(p->status == PROC_RUNNABLE);
    // we are exiting: detach from all kids.
    if(p->nkids) {
        for(int i = 1; i <= p->nkids; i++) {
            proc_t *k = p->kid_list[i];
            assert(k);
            k->parent = 0;
        }
    }

    // should we free?
    proc_t *parent = p->parent;
    if(parent) {
        assert(p->forked_cnt);
        p->status = PROC_EXITED;
    }

    // wait: this doesn't work if you fork.  ugh.
    if(p->respawn > 0) {
        assert(!p->parent);
        assert(!p->forked_cnt);

        static int n_respawn;

        p->respawn--;
        proc_respawn(ctx, p);
        if(!ctx->verbose_p) {
            if(n_respawn++ % 1024 == 0)
                output(".");
        }
    }
    assert(ctx);
    ctx->cur_proc = 0;
}

// save registers for current process and put it back on runqueue.
static inline void proc_save(pctx_t *ctx, uint32_t regs[17]) {
    assert(ctx);
    proc_t *p = ctx->cur_proc;
    assert(p);
    memcpy(p->regs, regs, 4 * 17);
    runq_enq(ctx, p);
    ctx->cur_proc = 0;
}

void switch_vm(pctx_t *ctx);

// run one instruction
static inline void proc_run_one(pctx_t *ctx) {
    assert(ctx);
    if(!(ctx->cur_proc = Q_pop(&ctx->runq))) {
        trace("all done with processes\n");
        pstats_print("done with processes", &ctx->stats, 1);
        clean_reboot();
        not_reached();
    }
    proc_t *p = ctx->cur_proc;
    if(p->status != PROC_RUNNABLE) {
        assert(p->status == PROC_BLOCKED);
    
        if(predicate_eval(p))
            output("succeeded! %d!\n", p->pid);
        else {
            output("skipping: putting back: %d!\n", p->pid);
            Q_append(&ctx->runq, p);
            proc_run_one(ctx);
            not_reached();
        }
        assert(p->status == PROC_RUNNABLE);
    }

    assert(p);
    assert(p->status == PROC_RUNNABLE);
    pstats_switch_inc(&ctx->stats, p);
    brkpt_mismatch_set(p->regs[15]);

    // this current code must be aliased and G=1
    switch_vm(ctx);

    switchto_user_asm(p->regs);
    not_reached();
}
#endif
