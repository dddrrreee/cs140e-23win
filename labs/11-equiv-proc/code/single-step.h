#ifndef __SINGLE_STEP_H__
#define __SINGLE_STEP_H__
#include "full-except.h"
#include "switchto.h"

typedef full_except_t single_handler_t;

// turn on single step mode.
//  - does any initialization needed.
//  - install <h> as the handler to call on a SS event.
//  - will trigger as soon as go to user mode
void single_step_on(single_handler_t h);

// turn off single step mode.
void single_step_off(void) ;

// low level routine: set a mismatch on <pc>: will
// run <pc> but will fault on anything else.
void single_mismatch_set(uint32_t pc);

// run one instruction in the process described by <regs>
void single_run_one(uint32_t regs[17]) __attribute__((noreturn));

// initialize <regs>
void single_step_mk_regs(uint32_t regs[17], uint32_t pc, uint32_t sp);

// run <fn> in single step mode.
void single_run_fn(single_handler_t h, int (*fn)(void), uint32_t sp) 
             __attribute__((noreturn));
#endif
