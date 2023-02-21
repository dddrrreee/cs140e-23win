/* engler, 140e: sorta cleaned up test code from 14-vm.  */
#include "rpi.h"
#include "rpi-constants.h"
#include "rpi-interrupts.h"

#include "vm-ident.h"
#include "mmu.h"


#if 0
// for today, just crash and burn if we get a fault.
void data_abort_vector(unsigned lr) {
    unsigned fault_addr;
    // b4-44
    asm volatile("MRC p15, 0, %0, c6, c0, 0" : "=r" (fault_addr));
    staff_mmu_disable();
    panic("data_abort fault at %x\n", fault_addr);
}

// shouldn't happen: need to fix libpi so we don't have to do this.
void interrupt_vector(unsigned lr) {
    staff_mmu_disable();
    panic("impossible\n");
}
#endif

/*
 * an ugly, trivial test run.
 *  1. setup the virtual memory sections.
 *  2. enable the mmu.
 *  3. do some simple tests.
 *  4. turn the mmu off.
 */ 
void vm_test(void) {
    fld_t *pt = vm_ident_mmu_init(1);
    assert(mmu_is_enabled());

    /*********************************************************************
     * 5. trivial test: write a couple of values, make sure they succeed.
     *
     * you should write additional tests!
     */

    // read and write a few values.
    int x = 5, v0, v1;
    assert(get32(&x) == 5);

    v0 = get32(&x);
    printk("doing print with vm ON\n");
    x++;
    v1 = get32(&x);


    /*********************************************************************
     * done with trivial test, re-enable.
     */
    vm_ident_mmu_off();
    printk("OFF\n");

    // 7. make sure worked.
    assert(v0 == 5);
    assert(v1 == 6);
    printk("******** success ************\n");
}

void notmain() {
    kmalloc_init_set_start(OneMB, OneMB);
    printk("implement one at a time.\n");
    check_vm_structs();
    vm_test();
}
