// part 1: uses your GPIO code to blink two LEDs where if one is off, the 
// other is on.
#include "rpi.h"

void notmain(void) {
    enum { led1 = 20, led2 = 21 };

    gpio_set_output(led1);
    gpio_set_output(led2);

    for(int i = 0; i < 10; i++) {
        gpio_set_on(led1);
        gpio_set_off(led2);
        delay_cycles(3000000);
        gpio_set_off(led1);
        gpio_set_on(led2);
        delay_cycles(3000000);
    }
}
