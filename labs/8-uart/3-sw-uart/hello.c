// the sw uart should  print hello world 10x if it works.
#include "rpi.h"
#include "sw-uart.h"

void notmain(void) {
    trace("about to use the sw-uart\n");
    trace("if your pi locks up, it means you are not transmitting\n");

    // turn off the hw UART so can use the same device.
    uart_disable();

    // use pin 14 for tx, 15 for rx
    sw_uart_t u = sw_uart_init(14,15, 115200);

    for(int i = 0; i < 10; i++)
        sw_uart_putk(&u, "TRACE: sw_uart: hello world\n");

    // reset to using the hardware uart.
    uart_init();
    trace("TRACE: done!\n");
}
