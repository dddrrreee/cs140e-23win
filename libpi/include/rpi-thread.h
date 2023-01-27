// engler,cs140e: trivial non-pre-emptive threads package.
#ifndef __RPI_THREAD_H__
#define __RPI_THREAD_H__

/*
 * trivial thread descriptor:
 *   - reg_save_area: space for all the registers (including
 *     the spsr).
 *   - <next>: pointer to the next thread in the queue that
 *     this thread is on.
 *  - <tid> unique thread id.
 *  - <stack>: fixed size stack: must be 8-byte aligned.  
 *
 * the big simplication: rather than save registers on the 
 * stack we stick them in a single fixed-size location at
 * offset in the thread structure.  
 *  - offset 0 means it's hard to mess up offset calcs.
 *  - doing in one place rather than making space on the 
 *    stack makes it much easier to think about.
 *  - the downside is that you can't do nested, pre-emptive
 *    context switches.  since this is not pre-emptive, we
 *    defer until later.
 *
 * changes:
 *  - dynamically sized stack.
 *  - save registers on stack.
 *  - add condition variables or watch.
 *  - some notion of real-time.
 *  - a private thread heap.
 *  - add error checking: thread runs too long, blows out its 
 *    stack.  
 */

// you should define these; also rename to something better.
#define REG_SP_OFF 36/4
#define REG_LR_OFF 40/4

#define THREAD_MAXSTACK (1024 * 8/4)
typedef struct rpi_thread {
    uint32_t *saved_sp;

	struct rpi_thread *next;
	uint32_t tid;

    // only used for part1: useful for testing without cswitch
    void (*fn)(void *arg);
    void *arg;          // this can serve as private data.
    
    const char *annot;
    // threads waiting on the current one to exit.
    // struct rpi_thread *waiters;

	uint32_t stack[THREAD_MAXSTACK];
} rpi_thread_t;
_Static_assert(offsetof(rpi_thread_t, stack) % 8 == 0, 
                            "must be 8 byte aligned");

// statically check that the register save area is at offset 0.
_Static_assert(offsetof(rpi_thread_t, saved_sp) == 0, 
                "stack save area must be at offset 0");

// main routines.



// starts the thread system: only returns when there are
// no more runnable threads. 
void rpi_thread_start(void);

// get the pointer to the current thread.  
rpi_thread_t *rpi_cur_thread(void);


// create a new thread that takes a single argument.
typedef void (*rpi_code_t)(void *);

rpi_thread_t *rpi_fork(rpi_code_t code, void *arg);

// exit current thread: switch to the next runnable
// thread, or exit the threads package.
void rpi_exit(int exitcode);

// yield the current thread.
void rpi_yield(void);


/***************************************************************
 * internal routines: we put them here so you don't have to look
 * for the prototype.
 */

// internal routine: 
//  - save the current register values into <old_save_area>
//  - load the values in <new_save_area> into the registers
//  reutrn to the caller (which will now be different!)
void rpi_cswitch(uint32_t **old_sp_save, const uint32_t *new_sp);

// returns the stack pointer (used for checking).
const uint8_t *rpi_get_sp(void);

// check that: the current thread's sp is within its stack.
void rpi_stack_check(void);

// do some internal consistency checks --- used for testing.
void rpi_internal_check(void);

#if 0
void rpi_wait(rpi_cond_t *c, lock_t *l);

// assume non-preemptive: if you share with interrupt
// will have to modify.
void rpi_lock(lock_t *l);
void rpi_unlock(lock_t *l);

static inline void lock(lock_t *l) {
    while(get32(l)) != 0)
        rpi_yield();
}
static inline void unlock(lock_t *l) {
    *l = 0;
}
#endif

// rpi_thread helpers
static inline void *rpi_arg_get(rpi_thread_t *t) {
    return t->arg;
}
static inline void rpi_arg_put(rpi_thread_t *t, void *arg) {
    t->arg = arg;
}
static inline unsigned rpi_tid(void) {
    rpi_thread_t *t = rpi_cur_thread();
    if(!t)
        panic("rpi_threads not running\n");
    return t->tid;
}


#endif
