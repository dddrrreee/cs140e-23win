/*
 * engler: trivial example to illustrate passing 32-bit ints from interrupt to 
 * client code.
 *
 * this should look very familiar, since it's mostly a less chatty version of 
 * timer.c from the previous lab.
 */
#include "rpi.h"
#include "rpi-inline-asm.h"
#include "timer-interrupt.h"
#include "libc/circular.h"

// make the circular queue wrap a few times.
static const unsigned max_push = CQ_N * 2;
static cq_t putQ;
static volatile unsigned push_fail, push_success, n_extra_ints;

// client has to define this.
void interrupt_vector(unsigned pc) {
    dev_barrier();
    unsigned pending = GET32(IRQ_basic_pending);

    // if this isn't true, could be a GPU interrupt (as discussed in Broadcom):
    // just return.  [confusing, since we didn't enable!]
    if((pending & RPI_BASIC_ARM_TIMER_IRQ) == 0) {
        n_extra_ints++;
        return;
    }

    // clear timer.
    PUT32(arm_timer_IRQClear, 1);

    // just push <max_push> of them.
    if(push_success < max_push) {
        // Q: what happens if we do not check?
        if(!cq_push32(&putQ, push_success))
            push_fail++;
        else
            push_success++;
    }

    dev_barrier();    
}

void notmain() {
    int_init();

    // if you mess with this value and prescaler, will change 
    // the number of interrupts.
    timer_interrupt_init(0x1);

    // Q: if you do this after enabling?
    cq_init(&putQ, 1);

    cpsr_int_enable();

    printk("enabled: will pass %d ints from int to client code\n", max_push);
    
    int i;
    for(i = 0; i < max_push; i++) {
        uint32_t x = cq_pop32(&putQ);
        if(x != i)
            panic("expected %d, have %d\n", i, x);
    }
    printk("DONE: read %d integers\n", i);
    printk("total success pushed =%d, total failed pushes=%d\n",
            push_success, push_fail);
    if(n_extra_ints)
        printk("extra interrupts: %d\n", n_extra_ints);
    else
        printk("NO extra interrupts\n");
    printk("SUCCESS!\n");
    clean_reboot();
}
