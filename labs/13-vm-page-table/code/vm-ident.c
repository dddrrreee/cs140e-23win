// What to do:
//   - replace the staff calls with calls to your routines in mmu.c
//
// section based: each section is 1MB.
//
// trivial setup of identity mappings where we map a virtual section 
// to the same physical section (e.g. 0x100000 maps to 0x100000)
//
// assumes a very simple address space:
//    - 1 section of code (starting at address 0x0)
//    - 1 section of user stack (ending at STACK_ADDR)
//    - 1 section of interrupt stack (ending at INT_STACK_ADDR)
//    - 1 section of heap (starting at one MB: 0x100000)
//    - 3 sections of GPIO ( 0x20000000 ---  0x20200000)
#include "vm-ident.h"
#include "vector-base.h"

// do one time initialization of MMU, setup identity mappings, and start
// running in mmu
fld_t * vm_ident_mmu_init(int start_p) {
    // 1. init
    staff_mmu_init();
    assert(!mmu_is_enabled());

    // ugly hack: we start by allocating a single page table at a known address.
    fld_t *pt = mmu_pt_alloc(4096);
    assert(pt == (void*)0x100000);

    // 2. setup mappings

    // map the first MB: shouldn't need more memory than this.
    staff_mmu_map_section(pt, 0x0, 0x0, dom_id);
    // map the heap/page table: for lab cksums must be at 0x100000.
    staff_mmu_map_section(pt, 0x100000,  0x100000, dom_id);

    // map the GPIO: make sure these are not cached and not writeback.
    // [how to check this in general?]
    todo("map GPIO (see procmap.h)");

    // setup user stack (note: grows down!)
    todo("map user stack (see procmap.h)");

    // setup interrupt stack (note: grows down!)
    // if we don't do this, then the first exception = bad infinite loop
    todo("map interrupt stack (see procmap.h)");

    // 3. install fault handler to catch if we make mistake.
    extern uint32_t default_vec_ints[];
    vector_base_set(default_vec_ints);

    // 4. start the context switch:

    // set up permissions for the one domain we use rn.
    staff_domain_access_ctrl_set(0b01 << dom_id*2);

    if(start_p)
        vm_ident_mmu_on(pt);
    return pt;
}

void vm_ident_mmu_on(fld_t *pt) {
    // this code uses the sequence on B2-25
    staff_set_procid_ttbr0(0x140e, dom_id, pt);

    // note: this routine has to flush I/D cache and TLB, BTB, prefetch buffer.
    staff_mmu_enable();
    assert(mmu_is_enabled());
}

void vm_ident_mmu_off(void) {
    staff_mmu_disable();
    assert(!mmu_is_enabled());
}
