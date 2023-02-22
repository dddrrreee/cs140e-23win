// do a write with insufficient privleges and reset them.
#include "vm-ident.h"
#include "libc/bit-support.h"

void vm_test(void) {
    proc.sp_lowest_addr = STACK_ADDR - OneMB;
    proc.dom_id = dom_id;
    proc.pt = vm_ident_mmu_init(1);
    assert(mmu_is_enabled());

    // 1: redo out of bounds stack write.
    // this is just to map it.
    volatile uint32_t *p = (void*)(STACK_ADDR - OneMB - 16);
    proc.fault_addr = (unsigned)p;
    *p = 11;
    assert(get32(p) == 11);

    // 2: do a write with insufficient privleges.
    printk("about to mark no access%x (vpn=%x)\n", p, mmu_sec_bits_only((uint32_t)p));
    uint32_t fa = proc.fault_addr = (unsigned)p;
    mmu_mark_sec_no_access(proc.pt, fa, 1);
    printk("HASH: marked no access%x (vpn=%x)\n", p, mmu_sec_bits_only(fa));
    assert(proc.fault_count == 1);
    put32(p, 12);
    assert(proc.fault_count == 2);
    assert(*p == 12);
    printk("HASH: done with invalid perm write to %x have=%d\n", p,*p);
    printk("HASH: test2: success!\n");

    // make sure the side-effect persists after
    vm_ident_mmu_off();
    assert(!mmu_is_enabled());
    assert(get32(p) == 12);
    printk("******** success ************\n");
}

void notmain() {
    mmu_be_quiet();
    kmalloc_init_set_start((void*)OneMB, OneMB);
    output("testing we can write and resume\n");
    check_vm_structs();
    vm_test();
}
