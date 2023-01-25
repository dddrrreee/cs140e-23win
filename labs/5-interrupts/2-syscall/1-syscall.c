/*
 * simple example of how to do a system call
 */
#include "rpi.h"
#include "rpi-interrupts.h"

// in syscall-asm.S
void run_user_code_asm(void (*fn)(void), void *stack);

// run <fn> at user level: <stack> must be 8 byte
// aligned
void run_user_code(void (*fn)(void), void *stack) {
    assert(stack);
    demand((unsigned)stack % 8 == 0, stack must be 8 byte aligned);

    todo("implement <run_user_code_asm> in syscall-asm.S!");
    run_user_code_asm(fn, stack);
    not_reached();
}

void interrupt_vector(unsigned pc) {
    panic("should not be called\n");
}


// example of using inline assembly to get the cpsr
// you can also use assembly routines.
static inline uint32_t cpsr_get(void) {
    uint32_t cpsr;
    asm volatile("mrs %0,cpsr" : "=r"(cpsr));
    return cpsr;
}

// pc should point to the system call instruction.
//      can see the encoding on a3-29:  lower 24 bits hold the encoding.
int syscall_vector(unsigned pc, uint32_t r0) {
    uint32_t inst, sys_num, mode;

    todo("check that spsr is USER level\n");
    // put spsr mode in <mode>
    if(mode != USER_MODE)
        panic("mode = %b: expected %b\n", mode, USER_MODE);
    else
        output("success: spsr is at user level\n");

    // we do a very trivial hello and exit just to show the difference
    switch(sys_num) {
    case 1: 
            printk("syscall: hello world\n");
            return 0;
    case 2: 
            printk("exiting!\n");
            clean_reboot();
    default: 
            printk("illegal system call = %d!\n", sys_num);
            return -1;
    }
}

// uses the same stack, just switches mode.
enum { N = 1024 * 64 };
uint64_t stack[N];

// this should be running at user level.
void user_fn(void) {
    uint64_t var;

    output("checking that stack got switched\n");
    assert(&var >= &stack[0]);
    assert(&var < &stack[N]);


    todo("check that the current mode is USER_LEVEL");
    // put the cpsr mode in <mode>
    unsigned mode = 0;
    if(mode != USER_MODE)
        panic("mode = %b: expected %b\n", mode, USER_MODE);
    else
        output("cpsr is at user level\n");

    output("about to call hello\n");
    asm volatile("swi 1" ::: "memory");

    output("about to call exit\n");
    asm volatile("swi 2" ::: "memory");

    not_reached();
}


void notmain() {
    todo("use int_init_vec to install vector with a different swi handler");

#if 0
    // you'll have to define these two symbols.
    extern uint32_t _int_table_user[], _int_table_user_end[];
    int_init_vec(_int_table_user, _int_table_user_end);
#endif

    todo("figure out a reasonable stack address using <stack>");
    uint64_t *sp = 0;

    output("calling user_fn with stack=%p\n", sp);
    run_user_code(user_fn, sp); 
    not_reached();
}
