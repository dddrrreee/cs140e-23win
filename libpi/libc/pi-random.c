#include <memory.h>
#include "rpi.h"
#include "random.h"
#include "pi-random.h"

#define STATESIZE 128
static int default_seed = 0;
static char statebuf[STATESIZE];
static struct random_data r;

static int initialized_p = 0;

// can redo using this.
void pi_random_seed(uint32_t x) {
    memset(&r, 0, sizeof r);
    if(initstate_r(x, statebuf, STATESIZE, &r))
        assert(0);
    if(srandom_r(x, &r))
        assert(0);
    initialized_p = 1;
}

// make sure that everyone has the same random.
uint32_t pi_random_init(void) {
    assert(!initialized_p);

    pi_random_seed(default_seed);
    unsigned u = pi_random();
    assert(0x6b8b4567 == u);
    return u;
}

uint32_t pi_random(void) {
// this is broken!  wow, nice.
//    DO_ONCE(pi_random_init());
    if(!initialized_p) {
        pi_random_init();
        initialized_p = 1;
    }

    int32_t x;
    if(random_r(&r, &x))
        assert(0);
    return x;
}
