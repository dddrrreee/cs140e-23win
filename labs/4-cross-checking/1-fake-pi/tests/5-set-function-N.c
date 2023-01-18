#include <stdio.h>
#include "test-helper.h"

void test_gpio_set_function(int ntrials) {
    printf("testing: <%s>\n", __FUNCTION__);
    // test pins 0..32, then a bunch of fake_random.
    for(int pin = 0; pin < 32; pin++)  {
        for(int func = 0;  func < 16; func++) {
            gpio_set_function(pin, func);
            gpio_set_function(fake_random(), func);
        }
    }
    for(int i = 0; i < ntrials; i++)
        gpio_set_function(fake_random(), fake_random()%6);
}



void notmain(void) {
#   define N 128
    test_gpio_set_function(N);
}
