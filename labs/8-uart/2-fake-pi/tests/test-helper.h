#include "rpi.h"
#include "fake-pi.h"



static inline void run_test(const char *name, void (*fp)(unsigned), int ntrials) {
    printf("testing: <%s>\n", name);
    // test pins 0..32, then a bunch of random.
    for(int i = 0; i < 32; i++)
        fp(i);
    for(int i = 0; i < ntrials; i++)
        fp(fake_random());
}

// can refactor this code, but we keep it simple.
static inline void test_gpio_set_output(int ntrials) {
    run_test(__FUNCTION__, gpio_set_output, ntrials);
}
static inline void test_gpio_set_input(int ntrials) {
    run_test(__FUNCTION__, gpio_set_input, ntrials);
}
static inline void test_gpio_set_on(int ntrials) {
    run_test(__FUNCTION__, gpio_set_on, ntrials);
}
static inline void test_gpio_set_off(int ntrials) {
    run_test(__FUNCTION__, gpio_set_off, ntrials);
}

