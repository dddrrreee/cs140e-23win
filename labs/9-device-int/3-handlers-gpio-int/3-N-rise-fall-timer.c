// last test: we test everything all at once: rise, fall, timer,
// all at once.
//
// DO NOT START WITH THIS TEST!   it pretty much requires everything
// works; too hard to debug otherwise.
#include "test-interrupts.h"

// check <N> fall/rise detections with timers.
static void test_N_all(unsigned N) {
    trace("should see %d falling edges!\n", N);

    assert(!n_falling);
    assert(!n_rising);
    assert(n_interrupt > 0);
    assert(gpio_read(in_pin) == 1);

    for(unsigned i = 1; i <= N; i++) {
        gpio_write(out_pin, 0);
        if(i != n_falling)
            panic("expected %d falling edges: have %d\n", 
                        i, n_falling);

        gpio_write(out_pin, 1);
        if(n_rising != i)
            panic("expected %d rising edges: have %d\n", 
                        i, n_rising);

        if(i && i % 1024 == 0)
            trace("passed %d tests so far [n_interrupt=%d]\n", i, n_interrupt);
    }
    // equal if no timer interrupts on.
    assert(n_falling <= n_interrupt);
    assert(n_rising <= n_interrupt);
}

void notmain() {
    trace("multiple rise/fall/timer test.\n", N);
    rise_fall_timer_int_startup();
    test_N_all(N);
    trace("SUCCESS: test passed!\n");
}
