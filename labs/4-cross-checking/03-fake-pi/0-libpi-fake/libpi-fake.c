#include "rpi.h"

// this only works because we are compiling a single routine 
// otherwise we'll get multiple definition errors.
int main(void) {
    notmain();
    return 0;
}
