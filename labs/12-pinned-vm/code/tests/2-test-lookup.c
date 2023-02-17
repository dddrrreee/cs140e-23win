/* Miles' little test for tlb_contains_va */

#include "rpi.h"
#include "pinned-vm.h"

void notmain(void) { 
    // map the heap: for lab cksums must be at 0x100000.
    kmalloc_init_set_start((void*)MB, MB);

    // turn on the pinned MMU: identity map.
    procmap_t p = procmap_default_mk(kern_dom);
    pin_mmu_on(&p);

    // if we got here MMU must be working b/c we have accessed GPIO/UART
    trace("hello: mmu must be on\n");

    lockdown_print_entries("before lookup");
    uint32_t result = 0;
    for(int i = 0; i < p.n; i++) {
        uint32_t addr = p.map[i].addr + 0xFAA0;
        if(!tlb_contains_va(&result, addr))
            panic("did not contai valid VA for %x\n", addr);

        if(!tlb_contains_va(&result, addr))
            panic("did not contai valid VA %x\n", addr);
        if(result != addr)
            panic("VA address didn't match!");
        trace("%x mapped to %x correctly\n", addr, result);
    }
    if(tlb_contains_va(&result, 0xDEADFAA0))
        panic("contained invalid VA\n");

    trace("SUCCESS!\n");
}
