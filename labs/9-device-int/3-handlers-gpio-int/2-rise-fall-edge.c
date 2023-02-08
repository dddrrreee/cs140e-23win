// simple test to check we are handling one rising and one falling
// edge interrupt correctly.
//
// NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., 
// have it "loopback")
#include "test-interrupts.h"

// just do a single falling edge: easier to debug.
static void test_one_fall_one_rise(void) {
    trace("about to test one rise/one fall\n");

    // checking <n_falling_edge> right away only works b/c 
    // <gpio_write()> takes so long that we are guaranteed 
    // the interrupt fired.  Good exercise: measure how
    // long the interrupt takes to propagate!
    n_interrupt = n_falling = n_rising = 0 ;
    assert(gpio_read(in_pin) == 1);

    gpio_write(out_pin, 0);
    if(!n_falling)
        panic("falling edge wrong: %d\n", n_falling);
    assert(n_falling == 1);
    assert(n_interrupt == 1);
    assert(n_rising == 0);
    trace("success: got a falling edge!\n");

    gpio_write(out_pin, 1);
    if(!n_rising)
        panic("rising=0: expected 1\n");
    assert(n_falling == 1);
    assert(n_rising == 1);
    assert(n_interrupt == 2);
    trace("success: got a rising edge!\n");
}

void notmain() {
    trace("test: single rise/fall edge detection.\n");

    // initialize interrupt stuff.  see <test-interrupts.c>
    trace("--------------------- interrupts now off ----------------\n");
    rise_fall_int_startup();
    trace("--------------------- interrupts now on ----------------\n");
    test_one_fall_one_rise();
    trace("SUCCESS: test passed!\n");
}
