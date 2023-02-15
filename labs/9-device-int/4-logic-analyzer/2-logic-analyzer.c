/*
 * engler, cs140e: simple tests to check that you are handling rising and falling
 * edge interrupts correctly.
 *
 * NOTE: make sure you connect your GPIO 20 to your GPIO 21 (i.e., have it "loopback")
 */
#include "rpi.h"
#include "timer-interrupt.h"
#include "libc/circular.h"
#include "sw-uart.h"
#include "cycle-count.h"

static cq_t uartQ;

enum { out_pin = 21, in_pin = 20 };
static volatile unsigned n_rising_edge, n_falling_edge;


// client has to define this.
void interrupt_vector(unsigned pc) {
    static uint32_t prev_pin_value = 1;
    unsigned curr_pin_value = gpio_read(in_pin);
    unsigned s  = cycle_cnt_read();

    dev_barrier();

    // Push the cycles since the last event into the queue.
    cq_push32(&uartQ, s);

    // Push the previous pin value into the queue. If the previous event was a falling edge,
    // the value pushed is 1. Otherwise, it's 0.
    cq_push32(&uartQ, prev_pin_value);

    // Update the previous pin value.
    prev_pin_value = curr_pin_value;

    gpio_event_clear(in_pin);

    dev_barrier();
    // 1. compute the cycles since the last event and push32 in the queue
    // 2. push the previous pin value in the circular queue (so if was
    //    falling edge: previous must have been a 1).
    //
    // notes:
    //  - make sure you clear the GPIO event!
    //  - using the circular buffer is pretty slow. should tune this.
    //    easy way is to use a uint32_t array where the counter is volatile. 
    //unimplemented();
}

void notmain() {
    cq_init(&uartQ,1);
    int_init();
    caches_enable();

    // use pin 20 for tx, 21 for rx
    sw_uart_t u = sw_uart_init(out_pin,in_pin, 115200);
    gpio_int_rising_edge(in_pin);
    gpio_int_falling_edge(in_pin);

    uint32_t cpsr = cpsr_int_enable();

    assert(gpio_read(in_pin) == 1);

    // assert(gpio_read(in_pin) == 1);

    // starter code.
    // make sure this works first, then try to measure the overheads.
    delay_ms(100);

    // this will cause transitions every time, so you can compare times.
    for(int l = 0; l < 2; l++) {
        unsigned b = 0b01010101;
        sw_uart_put8(&u, b);
        delay_ms(100);
        printk("nevent=%d\n", cq_nelem(&uartQ)/8);

        while(!cq_empty(&uartQ)) {
            unsigned ncycles    = cq_pop32(&uartQ);
            unsigned v          = cq_pop32(&uartQ);
            printk("\tv=%d, cyc=%d\n", v, ncycles);
        }
    }
    trace("SUCCESS!\n");
}
