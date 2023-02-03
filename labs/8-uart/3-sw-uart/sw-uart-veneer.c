/*
 * this shows how to do a "compatibility" layer to run programs meant
 * for one interface to run on another.   
 *   1. we implement uart routines in terms of our sw uart routines.
 *   2. if you link with this .o, it overrides any equivelent name
 *      in libpi.a (i.e., in our case: all the uart.o stuff).
 *   3. any call to uart routines will be forwarded to sw-uart.
 *   4. thus: *all previous tests that worked with uart should give the
 *      same results using sw-uart.*
 * 
 * the linking hack we depend on can be used for other things.
 *
 *
 * How to use: just link with this to flip in the sw-uart code for uart
 */
#include "rpi.h"
#include "sw-uart.h"

static sw_uart_t sw_uart;

int uart_put8(uint8_t c) {
    sw_uart_put8(&sw_uart, c);
    return 1;
}

// don't handle these for now.
int uart_get8(void) { unimplemented(); }
int uart_get8_async(void) { unimplemented(); }
int uart_has_data(void) { unimplemented(); }

// we don't buffer so can always do stuff.
int uart_can_put8(void) { return 1; }
void uart_flush_tx(void) {}

// we just ignore these.
void uart_disable(void) { }
void uart_destroy(void) {}

// default initilization as on the hw uart: pins 14 and 
// 15, default speed = 115200.
void uart_init(void) {
    sw_uart = sw_uart_init(14,15, 115200);
}
