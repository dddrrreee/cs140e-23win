// engler, cs140e: software uart interface.  the extra routines are mainly for 
// trying to read input fast enough without interrupts.
#ifndef __SW_UART_H__
#define __SW_UART_H__




// simple structure for sw-uart.  you'll likely have to extend in the future.
//  - rx: the GPIO pin used for receiving.  (make sure you set as input!)
//  - tx: the GPIO pin used for transmitting.  (make sure you set as output!)
//  - the baud rate: initially 115200, identical to what we already use.  if you
//      change this, you will also have to change the baud rate in pi-cat!
//  - cycle_per_bit: the number of cycles we have to hold the TX low (0) or 
//    high (1) to transmit a bit.  or, conversely, the number of cycles the 
//    RX pin will be held low or high by the sender.
typedef struct {
    uint8_t tx,rx;
    uint32_t baud;
    uint32_t cycle_per_bit;  // cycles between each bit.
    uint32_t usec_per_bit;   // microseconds b/n each bit.
} sw_uart_t;

/* 
 * We inline the computation for <cycle_per_bit> and <usec_per_bit>
 * b/c we don't have division on the pi.  division by a constant lets
 * the compiler statically compute result.
 */
#define sw_uart_init(_tx,_rx,_baud) \
    sw_uart_mk_helper(_tx,_rx, _baud, (700*1000*1000UL)/_baud, (1000*1000)/_baud)

/************************************************************************
 * Implement the following.
 */


// finish implementing this routine.  
sw_uart_t sw_uart_mk_helper(unsigned tx, unsigned rx, 
        unsigned baud, 
        unsigned cyc_per_bit,
        unsigned usec_per_bit);

// writes out a single byte <b>
void sw_uart_put8(sw_uart_t *uart, uint8_t b);


// returns -1 if no input after <timeout_usec>
//  - if timeout_usec = 0, will returns -1 right away if 
//    there is no data.
int sw_uart_get8_timeout(sw_uart_t *uart, uint32_t timeout_usec);

/************************************************************************
 * we give you the following code, which all calls your put8/get8 
 * implementations.
 */

// blocks until it gets a single byte and returns it.
// just calls sw_uart_get8_timeout with an infinite timeout.
int sw_uart_get8(sw_uart_t *uart);

// printk that uses sw_uart
int sw_uart_printk(sw_uart_t *uart, const char *fmt, ...);

// output a null-terminated string.
void sw_uart_putk(sw_uart_t *uart, const char *msg);

// read until <nbytes> or newline (deletes it). returns nbytes read.  
int sw_uart_gets(sw_uart_t *uart, char *in, unsigned nbytes);

// write <nbytes> of data held in <input> using <uart>
void sw_uart_write(sw_uart_t *uart, uint8_t *input, unsigned nbytes);

// blocking read of <nbytes> of data, and write into <out>.
void sw_uart_read(sw_uart_t *uart, uint8_t *out, unsigned nbytes);

// read in bytes into <out> until either we readin <nbytes> 
// of data or timeout after <timeout_usec>
//   - returns 0 (not -1) if nothing read.
int sw_uart_read_timeout(sw_uart_t *u, uint8_t *out,
                    uint32_t nbytes, uint32_t usec_timeout);

#endif
