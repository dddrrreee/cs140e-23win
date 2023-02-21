#include "vm-ident.h"
#include "libc/bit-support.h"

volatile struct proc_state proc;

// this is called by reboot: we turn off mmu so that things work.
void reboot_callout(void) {
    if(mmu_is_enabled())
        staff_mmu_disable();
}

void prefetch_abort_vector(unsigned lr) {
    unimplemented();
}

void data_abort_vector(unsigned lr) {
    unimplemented();
}
