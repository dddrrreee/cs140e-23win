

/*******************************************************************
 * simple process support.
 */
typedef struct proc {
    uint32_t regs[17+4];
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
uint32_t buf[1024];
static proc_t * volatile cur_proc = 0;
uint32_t buf2[1024];


#include "helper-macros.h"


static inline void 
proc_init(proc_t *p, void (*fn)(void), uint32_t expected_hash) {
    static int npid;
    assert((uint32_t)p%8==0);
    assert(is_aligned_ptr(p->regs, 8));
    p->regs[15] = (uint32_t)fn;
    p->regs[16] = USER_MODE;
    p->expected_hash = expected_hash;
    p->pid = ++npid;
}

static inline void 
proc_fork_stack(void (*fn)(void), uint32_t expected_hash) {
    enum { STACK_SIZE = 1024*64 };
    proc_t *p = kmalloc_aligned(STACK_SIZE, 16);
    proc_init(p, fn, expected_hash);
    p->regs[13] = (uint32_t)((char*)p+STACK_SIZE-2048);
    Q_append(&runq, p);
}

static inline void 
proc_fork_nostack(void (*fn)(void), uint32_t expected_hash) {
    proc_t *p = kmalloc_aligned(sizeof *p, 16);
    proc_init(p, fn, expected_hash);
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

#define curproc_get()  ({ assert(cur_proc); cur_proc; })

