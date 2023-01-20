#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    trace_fn(gpio_set_output(20));

    clean_reboot();
}
