#include "rpi.h"

void notmain(void) {
    debug("assert should fail:\n");
    int x = 10;
    assert(x < 4);
}
