#ifndef __EQUIV_OS_H__
#define __EQUIV_OS_H__

#include "rpi.h"
#include "breakpoint.h"
#include "proc.h"
#include "user-progs.h"

void dump_regs(pctx_t *ctx, uint32_t regs[17]) ;

void equiv_driver(pctx_t *ctx);

// initialize.
void sec_alloc_init(unsigned nsec);
// allocate exactly section <secn>
long sec_alloc_exact(uint32_t secn);
// free <secn>
void sec_free(uint32_t secn);

long sec_alloc(void);

int sec_is_legal(uint32_t secn);

long sec_alloc_exact_16mb(uint32_t secn);
long sec_alloc_16m(void);


#include "pinned-vm.h"

// how many times to respawn
enum { R = 10 };

#define dom_mask(x)  (DOM_client << ((x)*2))
enum {
    dom_kern = 1,
    dom_user = 2,
    dom_mask = dom_mask(dom_kern) | dom_mask(dom_user)
};


// start putting config options in here.
extern struct config {
    // if != 0: randomly flip a coin and 
    // Pr(1/random_switch) switch to another 
    // process in ss handler.
    uint32_t random_switch;

    // if 1: turn on cache [this will take work]
    unsigned do_caches_p:1;

    // if 1: turn vm off and on in the ss handler.
    unsigned do_vm_off_on:1;

    // if 1, randomly add to head or tail of runq.
    unsigned do_random_enq:1;

    unsigned emit_exit_p:1; // emit stats on exit.

    unsigned no_vm_p:1; // no VM
} config;

int do_random_enq(void);


static inline uint32_t sec_to_addr(uint32_t sec) 
    { return (sec << 20); }
static inline void* sec_to_ptr(uint32_t sec) 
    { return (void*)sec_to_addr(sec); }
static inline uint32_t addr_to_sec(uint32_t addr) 
    { return addr >> 20; }
static inline uint32_t ptr_to_sec(void *ptr) 
    { return addr_to_sec((uint32_t)ptr); }
static inline uint32_t ptr_to_off(void *ptr) 
    { return (uint32_t)ptr & ((1<<20)-1);}

// returns a *relative* pid
int sys_fork(pctx_t *ctx, uint32_t regs[17]);

void init(pctx_t *ctx);
proc_t *sys_exec(
        pctx_t *ctx,
        struct prog *prog,
        uint32_t expected_hash);


// you'll need to implement this unfortunately: clear the 
// pin at index <idx>
void pin_clear(unsigned idx);
void staff_pin_clear(unsigned idx);

#endif
