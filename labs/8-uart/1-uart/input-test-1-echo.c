// low level print that makes a bit easier to debug.
#include "rpi.h"
#include "memmap.h"

int get8_timeout(unsigned timeout_msec) {
    unsigned n = 1000 * timeout_msec;

    unsigned s = timer_get_usec();
    while((timer_get_usec() - s) < n)
        if(uart_has_data())
            return uart_get8();
    return -1;
}

void notmain(void) {
    caches_enable();
    uart_init();

    // shouldn't need more than 32MB
    // unbelievably sleavy: we just grab all the memory
    // after the program end: we do this b/c we can't 
    // take time in setup b/c the unix side is about to 
    // start blasting stuff.
    unsigned n = 32*1024*1024;
    uint8_t *buf = (void*)__heap_start__;

    // part 1: read the input into a buffer until
    // we see a pause of 100ms.
    int i;
    for(i = 0; i < n; i++) {
        int b;
        if((b = get8_timeout(100)) >= 0)
            buf[i] = b;
        else
            break;
    }
    if(i == n)
        panic("too much input: %d nbytes!\n", n);

    // part 2: now print the buffer out.
    n = i;
    for(i = 0; i < n; i++)
        uart_put8(buf[i]);

#if 0
    while(1) {
        uint8_t b;
        if((b = wait_for_data_ms(100)))
            uart_put8(b);
        else
            break;
    }
#endif
    printk("DONE!\n");
}
