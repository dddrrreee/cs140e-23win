// for this one: you would have to have a wire connecting 20 to 21.
// otherwise it won't work.
#include "rpi.h"
#include "trace.h"

void notmain(void) {
    uart_init();

    trace_start(0);
    const int led = 20;
    const int input = 21;

    gpio_set_output(led);
    gpio_set_input(input);
    for(int i = 0; i < 10; i++) {
        // could also do: 
        //  gpio_write(input, gpio_read(led));
        if(gpio_read(input))
            gpio_set_on(led);
        else
            gpio_set_off(led);
    }
    trace_stop();
    clean_reboot();
}
