// example client that uses our tracing system: can cross check these values
// against the ones on your laptop!
#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    trace_fn(gpio_write(20, 1));

    clean_reboot();
}
