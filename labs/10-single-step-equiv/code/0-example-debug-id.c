// check that we can get the debug out of cp14.
//
// gives an example of:
//   - how to define the inline assembly using our macros
//   - how to use structures with bitfields (see armv6-debug.h 
//     for the debug_id struct)
//   - crucially: how to use check bitfield offset/size checking macros.
//
// this "test" isn't one: should always just pass.
#include "rpi.h"
#include "armv6-debug.h"

void notmain(void) {
    // sanity check the debug_id structure.
    check_bitfield(struct debug_id, revision, 0, 4);
    check_bitfield(struct debug_id, variant, 4, 4);
    check_bitfield(struct debug_id, debug_rev, 12, 4);
    check_bitfield(struct debug_id, debug_ver, 16, 4);
    check_bitfield(struct debug_id, context, 20, 4);
    check_bitfield(struct debug_id, brp, 24, 4);
    check_bitfield(struct debug_id, wrp, 28, 4);

    // 13-6: get the debug id register value
    uint32_t r = cp14_debug_id_get();
    struct debug_id d = *(struct debug_id *)&r;

    trace("checking debug_id macro and inline function give same output\n");
    assert(r == cp14_debug_id_macro_get());

    // sanity check it using the values given on 13-7.
    trace("r=%x\n", r);
    assert(r>>12 == 0x15121);
    assert(bits_get(r, 12, 31) == 0x15121);

    // 13-7: number of watchpoint regs should be 2 (+1 the actual value)
    assert(bits_get(r, 28,31) == 0b001);
    assert(d.wrp == 0b1);
    trace("have %d watchpoints\n", d.wrp+1);

    // 13-7: number of breakpoint regs should be 6 (+1 the actual value)
    assert(bits_get(r, 24, 27) == 5);
    assert(d.brp == 0b101);
    trace("have %d breakpoints\n", d.brp+1);

    trace("SUCCESS: read debug register correctly.\n");
}
