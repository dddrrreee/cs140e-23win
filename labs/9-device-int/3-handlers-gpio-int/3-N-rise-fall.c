// last test: we test rise/fall without timer.  easiest of the 3-N tests.
#include "test-interrupts.h"

// check <N> fall/rise detections with timers.
static void test_N_rise_fall(unsigned N) {
    trace("should see %d falling edges!\n", N);

    assert(!n_falling);
    assert(!n_rising);
    assert(!n_interrupt);
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

        assert((n_rising + n_falling) == n_interrupt);
        assert(n_falling == n_rising);
    }
}

void notmain() {
    trace("multiple rise/fall [without timer] test.\n", N);
    rise_fall_int_startup();
    test_N_rise_fall(N);
    trace("SUCCESS: test passed!\n");
}
