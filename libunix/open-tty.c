// engler, cs140e.
#include <fcntl.h>
#include "libunix.h"

int open_tty_n(const char *device, int maxattempts) {
    int fd;
    for(int i = 0; i < maxattempts; i++) {
        if((fd = open(device, O_RDWR | O_NOCTTY | O_SYNC)) >= 0) {
            output("opened tty port <%s>.\n", device);
            return fd;
        }
        output("couldn't open tty port <%s>, going to sleep\n", device);
        sleep(1);
    }
    panic("couldn't open tty port <%s>\n", device);
}

    
// attempt to open tty <device>, retries some number of times if fails.
// otherwise panics.
int open_tty(const char *device) {
    const unsigned maxattempts = 5;
    return open_tty_n(device, maxattempts);
}
