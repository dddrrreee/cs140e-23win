#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    for(int i = 20; i < 27; i++)
        trace_fn(gpio_set_output(i));

    clean_reboot();
}
