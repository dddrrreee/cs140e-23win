// implement:
//  void uart_init(void)
//
//  int uart_can_get8(void);
//  int uart_get8(void);
//
//  int uart_can_put8(void);
//  void uart_put8(uint8_t c);
//
//  int uart_tx_is_empty(void) {
//
// see that hello world works.
//
//
#include "rpi.h"

// called first to setup uart to 8n1 115200  baud,
// no interrupts.
//  - you will need memory barriers, use <dev_barrier()>
//
//  later: should add an init that takes a baud rate.
#define GPIO_TX 14
#define GPIO_RX 15
//define all the registers for UART
enum
{
AUX_ENABLES = 0x20215004,
AUX_MU_IO_REG = 0x20215040,
 AUX_MU_IER_REG = 0x20215044,
 AUX_MU_IIR_REG = 0x20215048,
 AUX_MU_LCR_REG  = 0x2021504C,
 AUX_MU_MCR_REG = 0x20215050,
 AUX_MU_LSR_REG = 0x20215054,
 AUX_MU_MSR_REG = 0x20215058,
 AUX_MU_SCRATCH = 0x2021505C,
 AUX_MU_CNTL_REG = 0x20215060,
 AUX_MU_STAT_REG = 0x20215064,
 AUX_MU_BAUD_REG = 0x20215068,
};


void uart_init(void) { 
    dev_barrier();

    gpio_set_function(GPIO_TX, GPIO_FUNC_ALT5);
    gpio_set_function(GPIO_RX, GPIO_FUNC_ALT5);
    
    dev_barrier();

    PUT32(AUX_ENABLES, GET32(AUX_ENABLES) | 1);
    dev_barrier();

    PUT32(AUX_MU_CNTL_REG, 0);
    // dev_barrier();
    // Set GPIO pins 14 and 15 to Mini UART mode
   
    //dev_barrier();
    //gpio_set_func(GPIO_TX );
    //gpio_set_off(GPIO_RX );
    
   // dev_barrier();

    // Disable flow control and set data bits to 8
    
    //dev_barrier();

    // Disable interrupts
    PUT32(AUX_MU_IER_REG, 0);
    PUT32(AUX_MU_LCR_REG, 3);
    PUT32(AUX_MU_MCR_REG, 0);
    PUT32(AUX_MU_IIR_REG, 0b110);
    
   //
    
    //dev_barrier();

    // Set baud rate to 115200
    PUT32(AUX_MU_BAUD_REG, 270);
    //dev_barrier();

    // Enable transmitter and receiver

    PUT32(AUX_MU_CNTL_REG, 3);
    dev_barrier();
    //dev_barrier();
    //setting the pins to the correct mode ALT5
    //set the bit width to 8
    //disable flow control
    //check the 10th bit
}

// disable the uart.
void uart_disable(void) {
    // Disable Mini UART
    dev_barrier();
    uart_flush_tx();
    //dev_barrier();
    PUT32(AUX_ENABLES, (GET32(AUX_ENABLES) & ~1));
    dev_barrier();
    //uart_flush_tx();

}


// returns one byte from the rx queue, if needed
// blocks until there is one.
int uart_get8(void) {
    dev_barrier();
    while(!uart_has_data()) {
        ;
        // Wait for data to become available
    }
    //dev_barrier();
    unsigned der = GET32(AUX_MU_IO_REG) & 0xFF;
    
    dev_barrier();
    return (int)der;
	//return 0;
}

// 1 = space to put at least one byte, 0 otherwise.
int uart_can_put8(void) {
    //dev_barrier();
    return ((GET32(AUX_MU_STAT_REG) >> 1) & 0x1);
    // if(((GET32(AUX_MU_STAT_REG) >> 1) & 0x1) == 1){
    //     dev_barrier();
    //     return 1;
    // }
    //dev_barrier();
    //return  0;
    
    //return 0;
}

// put one byte on the tx qqueue, if needed, blocks
// until TX has space.
// returns < 0 on error.
int uart_put8(uint8_t c) {
    dev_barrier();
     while(!uart_can_put8()) {
        continue;
        // Wait for space to become available
    }
    PUT32(AUX_MU_IO_REG, c);
    //dev_barrier();
    return 0;

}

// simple wrapper routines useful later.

// 1 = at least one byte on rx queue, 0 otherwise
int uart_has_data(void) {
    //dev_barrier();
    //return ;
    if((GET32(AUX_MU_STAT_REG) & 0x1) == 1){
        dev_barrier();
        return 1;
    }
    //dev_barrier();
    return  0;
    //todo("must implement\n");
}

// return -1 if no data, otherwise the byte.
int uart_get8_async(void) { 
    if(!uart_has_data())
        return -1;
    return uart_get8();
}

// 1 = tx queue empty, 0 = not empty.
int uart_tx_is_empty(void) {
    dev_barrier();
    if (((GET32(AUX_MU_STAT_REG) >> 9) & 0x1) == 1)
    {
        dev_barrier();
        return 1;
    }
    dev_barrier();
    return 0;

    //return ((GET32(AUX_MU_STAT_REG) >> 9) & 1) = 1 ;
    //return ((GET32(AUX_MU_STAT_REG) >> 9) & 1) = 0;
    //dev_barrier();
    //unimplemented();
}

// flush out all bytes in the uart --- we use this when 
// turning it off / on, etc.
void uart_flush_tx(void) {
    dev_barrier();
    while(!uart_tx_is_empty())
        ;
    dev_barrier();    
}
