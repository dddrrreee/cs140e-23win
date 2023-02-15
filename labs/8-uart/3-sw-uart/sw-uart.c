// simple software 8n1 UART bit-banging implemention.
//   - look for todo() and implement!
//
// NOTE: using usec is going to be too slow for high baudrate, 
// but should be ok for 115200.
//
#include "rpi.h"
#include "cycle-count.h"
#include "sw-uart.h"
#include "cycle-util.h"

// simple putk to the given <uart>
void sw_uart_putk(sw_uart_t *uart, const char *msg) {
    for(; *msg; msg++)
        sw_uart_put8(uart, *msg);
}

// helper: cleans up the code: do a write for <usec> microseconds
//
// code that uses it will have a lot of compounding error, however.  
// if you switch to using cycles for faster baud rates, you should
// instead use
//      <write_cyc_until> in libpi/include/cycle-util.h
static inline void timed_write(int pin, int v, unsigned usec) {
    gpio_write(pin,v);
    delay_us(usec);
}

// do this first: used timed_write to cleanup.
//  recall: time to write each bit (0 or 1) is in <uart->usec_per_bit>
//time it in terms of cycce; ms are not granular alone; cycles/bit
void sw_uart_put8(sw_uart_t *uart, unsigned char c) {
    unsigned sttime = cycle_cnt_read();
    write_cyc_until(uart->tx, 0,sttime, uart->cycle_per_bit);
    //timed_write(uart->tx, 0, uart->usec_per_bit);

    // send the data bits
    for (int i = 0; i < 8; i++) {
        int bit = (c >> i) & 1;
        sttime += (uart -> cycle_per_bit);
        write_cyc_until(uart->tx, bit ,sttime, uart->cycle_per_bit);
        //timed_write(uart->tx, bit, uart->usec_per_bit);
    }

    // send the stop bits
    write_cyc_until(uart->tx, 0,sttime + (uart->cycle_per_bit), uart->cycle_per_bit);
   // timed_write(uart->tx, 1, uart->usec_per_bit);
    //todo("implement this routine");
}

// do this second: you can type in pi-cat to send stuff.
//      EASY BUG: if you are reading input, but you do not get here in 
//      time it will disappear.
int sw_uart_get8(sw_uart_t *uart) {
    // int bit_time = uart->usec_per_bit;
    // unsigned char c = 0;

    // // wait for start bit
    // while(gpio_read(uart->rx) == 1)
    //     delay_us(bit_time);

    // // read each bit
    // for(int i = 0; i < 8; i++) {
    //     delay_us(bit_time/2);
    //     int bit = gpio_read(uart->rx);
    //     c |= bit << (7-i);
    //     delay_us(bit_time/2);
    // }

    // // wait for stop bit
    // delay_us(bit_time);

    // return c;
    unimplemented();
    //todo("getc is an extension");
}

// setup the GPIO pins
sw_uart_t sw_uart_mk_helper(unsigned tx, unsigned rx,
        unsigned baud,
        unsigned cyc_per_bit,
        unsigned usec_per_bit) {

    // implement:
    //  1. set rx and tx as input/output.
    //  2: what is the default value of tx for 8n1?  make sure
    //     this is set!!
    //tx output rx ouptut: tx high
    //luca 
    gpio_set_function (tx, GPIO_FUNC_OUTPUT);
    gpio_set_function (rx, GPIO_FUNC_OUTPUT);
    //gpio_set_output(tx);
    //gpio_set_input(rx);
    gpio_write(tx, 1);
    //todo("set up the right GPIO pins with the right values");

    // check that the given values make sense.
    //
    // we give you the rest.
    // make sure the value makes sense.
    unsigned mhz = 700 * 1000 * 1000;
    unsigned derived = cyc_per_bit * baud;
    assert((mhz - baud) <= derived && derived <= (mhz + baud));
    // panic("cyc_per_bit = %d * baud = %d\n", cyc_per_bit, cyc_per_bit * baud);

    return (sw_uart_t) {
            .tx = tx,
            .rx = rx,
            .baud = baud,
            .cycle_per_bit = cyc_per_bit ,
            .usec_per_bit = usec_per_bit
    };
}
