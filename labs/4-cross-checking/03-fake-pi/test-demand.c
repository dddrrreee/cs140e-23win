#include "rpi.h"

void notmain(void) {
    debug("demand should fail\n");
    int x = 10;
    demand(x < 4, testing demand: this should fail);
}
