// rise+fall+timer.
//
// NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., 
// have it "loopback")
#include "test-interrupts.h"

// just do a single falling edge: easier to debug.
static void test_fall_rise_timer(void) {
    // measure a single rising edge.
    trace("about to test a single falling edge\n");
    // checking <n_falling_edge> right away only works b/c 
    // <gpio_write()> takes so long that we are guaranteed 
    // the interrupt fired.  Good exercise: measure how
    // long the interrupt takes to propagate!
    if(!n_interrupt)
        panic("timer interrupts not on?\n");

    assert(!n_falling);
    assert(!n_rising);
    assert(gpio_read(in_pin) == 1);

    n_interrupt = 0;
    gpio_write(out_pin, 0);
    if(!n_falling)
        panic("falling edge wrong: %d\n", n_falling);
    assert(n_falling == 1);
    assert(n_interrupt >= 1);
    assert(n_rising == 0);
    trace("success: got a falling edge [n_interupt=%d]!\n", n_interrupt);

    gpio_write(out_pin, 1);
    if(!n_rising)
        panic("rising=0: expected 1\n");
    assert(n_falling == 1);
    assert(n_rising == 1);
    assert(n_interrupt >= 2);
    trace("success: got a rising edge! [n_interrupts=%d]\n", n_interrupt);
}

void notmain() {
    trace("test: single rise/fall edge detection with timer ints.\n");

    // initialize interrupt stuff.  see <test-interrupts.c>
    trace("--------------------- interrupts now off ----------------\n");
    rise_fall_timer_int_startup();
    trace("--------------------- interrupts now on ----------------\n");
    test_fall_rise_timer();
    trace("SUCCESS: test passed [total interrupts=%d]!\n", n_interrupt);
}
