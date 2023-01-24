#include "rpi.h"

uint32_t cpsr_get(void);
void* sp_get(void);
void run_fn_at_system(void (*fn)(void), void *sp);

enum { N = 1024 * 64 / 8 };
static uint64_t stack[N];

void hello(void) {
    uint64_t *sp = sp_get();
    printk("hello: running with stack: sp = %x, &stack[N]=%x\n", 
            sp, &stack[N]);

    assert(sp < &stack[N]);
    assert(sp >= &stack[0]);
}

void notmain(void) {
    uint32_t cpsr = cpsr_get();
    printk("cpsr = <%b>\n", cpsr);
    printk("    mode = <%b>\n", cpsr&0b11111);
    printk("    interrupt = <%s>\n", 
                ((cpsr >> 7)&1) ? "off" : "on");

    // swap this around if you want to see that it works.
#if 1
    run_fn_at_system(hello, stack);
#else
    run_fn_at_system(hello, &stack[N]);
#endif
    not_reached();
}
