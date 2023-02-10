// simple test of timer interrupts.
//
// NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., 
// have it "loopback")
#include "test-interrupts.h"

// just do a single falling edge: easier to debug.
static void test_timer(void) {
    // measure a single rising edge.
    trace("about to test timer ints\n");
    assert(!n_falling);
    assert(!n_rising);

    assert(n_interrupt > 0);
    n_interrupt = 0;
    delay_ms(10);
    assert(n_interrupt > 0);
    trace("delay_ms(10) = [%d] timer interrupts\n", n_interrupt);

    assert(n_falling == 0);
    assert(n_rising == 0);
    trace("success: timer interrupts!\n");
}

void notmain() {
    trace("test: single falling edge detection.\n");

    // initialize interrupt stuff.  see <test-interrupts.c>
    trace("--------------------- interrupts now off ----------------\n");
    timer_int_startup();
    trace("--------------------- interrupts now on ----------------\n");
    test_timer();
    trace("SUCCESS: test passed!\n");
}
