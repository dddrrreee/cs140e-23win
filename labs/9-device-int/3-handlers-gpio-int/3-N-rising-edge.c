// test that we are detecting N rising edges correctly.
//
// NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., 
// have it "loopback")
#include "test-interrupts.h"

static void test_N_rising(unsigned N) {
    trace("should see %d rising edges!\n", N);
    assert(!n_falling);
    assert(!n_rising);
    assert(!n_interrupt);
    assert(gpio_read(in_pin) == 0);

    for(unsigned i = 1; i <= N; i++) {
        gpio_write(out_pin, 1);

        // again: relying on how slow gpio_write() is.
        // this isn't good practice!
        if(i != n_rising)
            panic("expected %d rising edges: have %d\n", i, n_rising);

        gpio_write(out_pin, 0);
        assert(n_rising == n_interrupt);
    }
}


void notmain() {
    trace("%d rising edge detection test.\n", N);
    rising_int_startup();
    trace("interrupts enabled\n");
    test_N_rising(N);
    assert(n_rising == n_interrupt);
    trace("SUCCESS: test passed!\n");
}
