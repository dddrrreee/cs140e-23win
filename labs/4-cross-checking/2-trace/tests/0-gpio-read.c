// example client that uses our tracing system: can cross check these values
// against the ones on your laptop!
#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    printk("about to trace gpio_read(20):\n");
    trace_start(0);
    int v = gpio_read(20);
    trace_stop();
    printk("done: gpio_read(20)=%d\n", v);

    clean_reboot();
}
