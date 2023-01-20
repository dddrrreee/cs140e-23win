// rewritten 1-blink.c from the last lab: may want to cut down on delay.
#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    trace_start(0);

    int led = 20;
    gpio_set_output(led);
    for(int i = 0; i < 10; i++) {
        gpio_set_on(led);
        delay_cycles(1000000);
        gpio_set_off(led);
        delay_cycles(1000000);
    }

    trace_stop();
    clean_reboot();
}
