// low level print that makes a bit easier to debug.
#include "rpi.h"
#include "memmap.h"


int get8_timeout(unsigned timeout_msec) {
    unsigned n = 1000 * timeout_msec;

    unsigned s = timer_get_usec();
    while((timer_get_usec() - s) < n)
        if(uart_has_data())
            return uart_get8();

    panic("uart had no data for %dms: did you pipe it into my-install?\n", 
                timeout_msec);
}

// test that we can read a single line of input.
void notmain(void) {
    uart_init();

    char buf[1024];
    for(int i = 0; i < sizeof buf; i++) {
        buf[i] = get8_timeout(300);
        if(buf[i] == '\n') {
            buf[i] = 0;
            break;
        }
    }
    output("TRACE: received: <%s>\n", buf);
}
