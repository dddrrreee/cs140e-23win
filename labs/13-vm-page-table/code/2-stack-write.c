// see that you can grow the stack.
#include "vm-ident.h"
#include "libc/bit-support.h"

void vm_test(void) {
    // setup a simple process structure that tracks where the stack is.
    proc.sp_lowest_addr = STACK_ADDR - OneMB;
    proc.dom_id = dom_id;
    proc.pt = vm_ident_mmu_init(1);

    // datafault handler is in mmu-except.c

    // 1: out of bounds stack write: you should grow the stack (add a mapping)
    //    and resume.
    // we know this is not mapped.
    volatile uint32_t *p = (void*)(STACK_ADDR - OneMB - 16);
    printk("HASH: about to do an invalid write to %x\n", p);
    proc.fault_addr = (unsigned)p;
    put32(p, 11);
    assert(get32(p) == 11);
    assert(get32(p) == *p);
    printk("HASH:done with invalid write to %x have=%d\n", p,*p);
    printk("HASH:test1: success\n");

    // do we still have the value after disabling?
    vm_ident_mmu_off();
    assert(!mmu_is_enabled());

    assert(get32(p) == 11);
    printk("******** success ************\n");
}

void notmain() {
    kmalloc_init_set_start(OneMB, OneMB);
    output("checking that stack gets extended\n");
    vm_test();
}
