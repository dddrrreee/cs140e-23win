// test of gets_until: enter lines in pi-cat.  if you hit newline will send to the pi
#include "rpi.h"
#include "sw-uart.h"

void notmain(void) {
    uart_init();
    enable_cache();

    // use pin 20 for tx, 21 for rx
    sw_uart_t u = sw_uart_mk(20,21, 115200);

    unsigned n = 2;

    printk("about to get %d lines: make sure you've started pi-cat\n", n);

    for(int i = 0; i < n; i++) {
        printk("iter=%d, going to do a get of a whole line: type in the pi-cat window!\n", i);

        // this will be on the pi-cat side.
        sw_uart_printk(&u, "enter a line: ");

        char buf[1024];
        int res = sw_uart_gets(&u, buf, sizeof buf-1);
        assert(strlen(buf) == res);
        // kill newline.
        buf[res-1] = 0;
        printk("SW-UART: got string <%s>\n", buf);
    }

    printk("done!\n");
    clean_reboot();
}
