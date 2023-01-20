#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    trace_fn(gpio_set_input(20));

    clean_reboot();
}
