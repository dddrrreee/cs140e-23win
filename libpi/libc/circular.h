// use memory barriers.  volatile is really bad.
#ifndef __CQ2_H__
#define __CQ2_H__

// simple lock-free circular queue.
#ifndef RPI_UNIX
#   include "rpi.h"
#   include "rpi-interrupts.h"
#   include "rpi-inline-asm.h"
#else
#   define printk printf
#   include <assert.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   define int_is_enabled() 0
//#   include "demand.h"
#endif


// simple circular FIFO Q.   
//
// if we want to store records?
//
// TODO:
//  - also should extend so it works with threads.
//  - should symex test this.   "How to write code to execute on a symbolic CPU"
//
// could simplify the interface if we wanted to only allow < 32bit valuesto be stored.


// make the code templatized on this? (ala fraser and hanson)  
#ifndef CQE_T
    typedef unsigned char cqe_t;
#else
    typedef CQE_T cqe_t;
#endif

// XXX: i think change this so you can size the buffers to what you think makes sense.
// e.g., for output we probably want a huge one.  or dynamically adapt.   if you want
// stack allocation they could pass in pointers to the buffer, etc.
//
// hardcoding N means we can stack allocate everything if needed.  (avoiding dynamic
// allocation seems like something worthwhile in an embedded system, but in our case
// perhaps is not relevant since we have the trivial kmalloc pointer bump.)
#ifndef CQ_N
#   define CQ_N 8192
#endif
typedef struct {
    // single mutator of head, single mutator of tail = lock free.  
    cqe_t c_buf[CQ_N];
    unsigned fence;
    unsigned head, tail;

    // number of times a push failed b/c there were more than N elements.
    unsigned overflow;
    // do we panic on error?
    unsigned errors_fatal_p:1;
} cq_t;


/*************************************************************************************
* inline these so they can be used when we bit-bang off the UART with no interrupts.
*/
static inline int cq_empty(cq_t *q) { gcc_mb(); return q->head == q->tail; }

// if adding 1 to head = tail, then we have no space.
static inline int cq_full(cq_t *q) { gcc_mb(); return (q->head+1) % (CQ_N) == q->tail; }

static inline unsigned cq_nelem(cq_t *q) { gcc_mb(); return (q->head - q->tail) % (CQ_N); }
static inline unsigned cq_nspace(cq_t *q) { return (CQ_N-1) - cq_nelem(q); }

// not blocking: requires interrupts.
static inline int cq_pop_nonblock(cq_t *c, cqe_t *e) {
    if(cq_empty(c))
        return 0;

    // there is a memory barrier in cq_eq
    unsigned tail = c->tail;
    // must occur in order.
    *e = c->c_buf[tail];     // 1
    gcc_mb();
    c->tail = (tail+1)% (CQ_N);  // 2
    gcc_mb();
    return 1;
}
// blocking: called from non-interrupt code.
static inline cqe_t cq_pop(cq_t *c) {
    cqe_t e = 0;

	// wait til interrupt puts something here: if interrupts not enabled,
    // this will deadlock: need to yield.
    while(!cq_pop_nonblock(c,&e)) {
        // at this point you would call rpi_wait()
        if(!cpsr_int_enabled())
            panic("will deadlock: interrupts not enabled [FIXME]\n"); 
    }
    return e;
}

// non-blocking push: returns 0 if full.
static inline int cq_push(cq_t *c, cqe_t x) {
    if(cq_full(c)) 
        return 0;
    gcc_mb();
    unsigned head = c->head;
    assert(cq_nspace(c) > 0);
    // must occur in order
    c->c_buf[head] = x;  // 1
    gcc_mb();
    c->head = (head + 1) % (CQ_N); // 2
    gcc_mb();
    return 1;
}

// non-destructively peek at the first character (if any).
static inline int cq_peek(cq_t *c, cqe_t *e) {
    if(cq_empty(c))
        return 0;
    gcc_mb();
    *e = c->c_buf[c->tail];
    gcc_mb();
    return 1;
}

// XXX: we could consolidate all the memory barrier stuff.
static inline void cq_pop_n(cq_t *c, void *data, unsigned n) {
    cqe_t *p = data;
    for(int i = 0; i < n; i++)
        p[i] = cq_pop(c);
    gcc_mb();
}
// XXX: we could consolidate all the memory barrier stuff.
static inline int cq_pop_n_noblk(cq_t *q, void *data, unsigned n) {
    if(cq_nelem(q) < n)
        return 0;
    cq_pop_n(q,data,n);
    return 1;
}

// XXX: we could consolidate all the memory barrier stuff.
// non-destructively peek ahead <n> entries.
static inline int cq_peek_n(cq_t *c, cqe_t *v, unsigned n) {
    int sz = cq_nelem(c);
    if(n > sz)
        return 0;
    gcc_mb();
    unsigned tail = c->tail;
    gcc_mb();
    if(!cq_pop_n_noblk(c, v, n))
        return 0;
    gcc_mb();
    c->tail = tail;
    gcc_mb();
    assert(sz == cq_nelem(c));
    return 1;
}


// XXX: we could consolidate all the memory barrier stuff.
static inline int cq_push_n(cq_t *c, const void *data, unsigned n) {
    assert(n);

    const cqe_t *p = data;

    if(cq_nspace(c) < n)
        return 0;
    gcc_mb();
    for(int i = 0; i < n; i++)
        if(!cq_push(c, p[i]))
            panic("impossible: buffer should have space\n");

    // assert(!cq_empty(c));
    return 1;
}

// example of how to wrap up so you can easily push larger
// things.

// returns 0 if failed, 1 otherwise.
static inline int cq_push32(cq_t *c, uint32_t x) {
    return cq_push_n(c, &x, sizeof x);
}
// returns 0 if failed, 1 otherwise
static inline int cq_pop32_noblk(cq_t *c, uint32_t *x) {
    return cq_pop_n_noblk(c, x, sizeof *x);
}
// returns a uint32: blocks until one is available.
static inline uint32_t cq_pop32(cq_t *c) {
    uint32_t x = 0;
    cq_pop_n(c,&x, sizeof x);
    return x;
}

/**********************************************************************
 * initialization.
 */
static inline void cq_print(cq_t *c) {
    debug("cq print: head=%d, tail=%d, nelem=%d, empty=%d\n", 
                c->head, c->tail, cq_nelem(c), cq_empty(c));
}
static inline void cq_ok(cq_t *c) {
    if(c->fence != 0x12345678)
        panic("fence is corrupted\n");
}
static inline void cq_init(cq_t *c, unsigned errors_fatal_p) {
    memset((void*)c, 0, sizeof *c);
    c->fence = 0x12345678;
    c->head = c->tail = 0;
    c->overflow = 0;
    c->errors_fatal_p = errors_fatal_p;
    assert(cq_empty(c));
    assert(!cq_full(c));
    assert(cq_nelem(c) == 0);
    cqe_t e = 0x12;
    assert(cq_pop_nonblock(c,&e) == 0 && e == 0x12);
}
static inline unsigned cq_ckpt(cq_t *c) { return c->tail; }
static inline void cq_restore(cq_t *c, unsigned ckpt) { c->tail = ckpt; }
#endif
