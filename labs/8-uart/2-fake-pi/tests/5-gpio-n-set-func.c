#include "rpi.h"
void notmain(void) {
    for(int i = 0; i < 7; i++)
        gpio_set_function(20, i);
}
