// test that we die when we jump to memory that has no permissions.
#include "vm-ident.h"
#include "libc/bit-support.h"

void vm_test(void) {
    proc.sp_lowest_addr = STACK_ADDR - OneMB;
    proc.dom_id = dom_id;
    proc.pt = vm_ident_mmu_init(1);
    assert(mmu_is_enabled());


    output("should die with a message about a permission error\n");
    uint32_t unmapped = (STACK_ADDR +  4*OneMB);
    proc.die_addr = unmapped;
    staff_mmu_map_section(proc.pt, unmapped, unmapped, proc.dom_id);
    mmu_mark_sec_no_access(proc.pt, unmapped, 1);

    void (*fp)(void) = (void*)unmapped;
    fp();
    panic("should not be reached\n");
}

void notmain() {
    mmu_be_quiet();
    kmalloc_init_set_start((void*)OneMB, OneMB);
    output("testing we die if jump to unmmapped memory\n");
    check_vm_structs();
    vm_test();
}
