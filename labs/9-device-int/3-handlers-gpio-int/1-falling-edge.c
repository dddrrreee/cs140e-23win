// simple test to check we are handling falling edge interrupts
// correctly.
//
// NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., 
// have it "loopback")
#include "test-interrupts.h"

// just do a single falling edge: easier to debug.
static void test_one_falling(void) {
    // measure a single rising edge.
    trace("about to test a single falling edge\n");
    assert(!n_interrupt);
    assert(!n_falling);
    assert(!n_rising);
    assert(gpio_read(in_pin) == 1);


    // checking <n_falling_edge> right away only works b/c 
    // <gpio_write()> takes so long that we are guaranteed 
    // the interrupt fired.  Good exercise: measure how
    // long the interrupt takes to propagate!
    gpio_write(out_pin, 0);
    if(!n_falling)
        panic("falling edge wrong: %d\n", n_falling);
    if(n_falling != n_interrupt)
        panic("falling=%d, n_interrupt=%d\n", n_falling, n_interrupt);


    assert(n_falling == 1);
    assert(n_interrupt == 1);
    trace("success: got a falling edge!\n");
}

void notmain() {
    trace("test: single falling edge detection.\n");

    // initialize interrupt stuff.  see <test-interrupts.c>
    trace("--------------------- interrupts now off ----------------\n");
    falling_int_startup();
    trace("--------------------- interrupts now on ----------------\n");
    test_one_falling();
    trace("SUCCESS: test passed!\n");
}
