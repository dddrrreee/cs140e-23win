// rewritten 2-blink.c from 1-gpio
#include "rpi.h"
#include "trace.h"

void notmain(void) {
    enum { led1 = 20, led2 = 21 };

    uart_init();
    trace_start(0);

    gpio_set_output(led1);
    gpio_set_output(led2);

    for(int i = 0; i < 10; i++) {
        gpio_set_on(led1);
        gpio_set_off(led2);
        delay_cycles(1000000);
        gpio_set_off(led1);
        gpio_set_on(led2);
        delay_cycles(1000000);
    }
    trace_stop();
    clean_reboot();
}
