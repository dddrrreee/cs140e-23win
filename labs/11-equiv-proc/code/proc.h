

/*******************************************************************
 * simple process support.
 */
typedef struct proc {
    uint32_t regs[17];
    uint32_t expected_hash;
    uint32_t reg_hash;
    volatile uint32_t pid;
    uint32_t inst_cnt;
    struct proc *next;
} proc_t;
#define E proc_t
#include "Q.h"

// very bad form.
static Q_t runq;
static proc_t * volatile cur_proc = 0;

static inline void proc_fork(void (*fn)(void), uint32_t expected_hash) {
    static int npid;
    enum { STACK_SIZE = 1024*64 };
    proc_t *p = kmalloc_aligned(STACK_SIZE, 16);
    p->regs[13] = (uint32_t)((char*)p+STACK_SIZE);
    p->regs[15] = (uint32_t)fn;
    p->regs[16] = USER_MODE;
    p->expected_hash = expected_hash;
    p->pid = ++npid;
    Q_append(&runq, p);
}

void switchto_user_asm(uint32_t regs[16]);

static void proc_exit(void) {
    cur_proc = 0;
}

static inline void proc_save(uint32_t regs[17]) {
    assert(cur_proc);
    memcpy(cur_proc->regs, regs, 4 * 17);
    Q_append(&runq, cur_proc);
    cur_proc = 0;
}

static inline void proc_run_one(void) {
    if(!(cur_proc = Q_pop(&runq))) {
        trace("all done with processes\n");
        clean_reboot();
        not_reached();
    }
    assert(cur_proc);
    brkpt_mismatch_set(cur_proc->regs[15]);
    switchto_user_asm(cur_proc->regs);
    not_reached();
}

static inline proc_t *curproc_get(void) { 
    assert(cur_proc);
    return cur_proc;
}
