// check that null and alignment is handled.
#include "rpi.h"
#include "vector-base.h"

// in interrupt-asm.S
void bad_exception_asm(void);

void notmain(void) {
    // exception vector that we don't ever call through.
    uint32_t vector[8] __attribute__ (( aligned (64) ));

    trace("aligned, non-null vector base: should be ok: ");
    if(!vector_base_chk(vector))
        panic("vector %p falsely flagged as illegal\n", vector);
    else
        output("passed\n");

    trace("unaligned vector base: should detect: ");
    if(vector_base_chk(&vector[1]))
        panic("vector %p should be rejected\n", vector);
    else
        output("passed\n");

    trace("about to set null vector base: should detect: ");
    if(vector_base_chk(0))
        panic("vector %p should be rejected\n", 0);
    else
        output("passed!\n");

    trace("about to set vector base: ");
    vector_base_set(vector);

    void *v = vector_base_reset(vector);
    if(v != vector)
        panic("reset did not work? %p != %p\n", v, vector);
    else
        output("passed!\n");

    trace("success\n");
}
