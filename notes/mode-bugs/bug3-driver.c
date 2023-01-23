#include "rpi.h"

void switch_to_system_w_stack(void *sp);
uint32_t cpsr_get(void);
uint32_t sp_get(void);

enum { N = 1024 * 64 / 8 };
// we used unsigned long long so is 8 byte aligned.
static unsigned long long stack[N];

void notmain(void) {
    uint32_t cpsr = cpsr_get();
    printk("cpsr = <%b>\n", cpsr);
    printk("    mode = <%b>\n", cpsr&0b11111);
    printk("    interrupt = <%s>\n", 
                ((cpsr >> 7)&1) ? "off" : "on");


    uint32_t base = (uint32_t)&stack[0];
    demand(base % 8 == 0, "stack %p is not 8 byte aligned!\n", base);

    // switch to system using the given stack.
    switch_to_system_w_stack(stack);
    printk("switched to system: sp = %x\n", sp_get());
}
