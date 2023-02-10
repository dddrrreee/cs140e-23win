// example of how to use the bitfield helpers.
#include "rpi.h"
#include "armv6-debug.h"

void notmain(void) {
    uint32_t u = ~0;

    // check that bits 0..31 of ~0 are 1.
    assert(bits_eq(u, 0, 31, ~0));

    // set all bits from 0...31 --- check = ~0
    assert(bits_set(0, 0, 31, ~0) == u);

    // clear bits 0..31 in ~0: check that this = 0.
    assert(bits_clr(~0, 0, 31) == 0);

    for(int i = 0;  i < 32; i++) {
        assert(bit_isset(u,i));

        assert(bits_get(u,i, i) == 1);
        u = bit_clr(u,i);
        assert(bits_get(u,i, i) == 0);
        assert(!bit_isset(u,i));
        u = bit_set(u,i);
        assert(bit_isset(u,i));
        assert(bits_get(u,i, i) == 1);
    }

    // set all 16 bits to 1.
    unsigned short us = ~0;
    // cast to uint32: upper bits are 0
    assert(bits_eq(us, 16, 31, 0));
    // and lower 16 bits are 1
    assert(bits_eq(us, 0, 15, us));
    trace("simple bit tests passed.\n");
}
