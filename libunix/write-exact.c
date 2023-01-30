#include "libunix.h"

int write_exact(int fd, const void *data, unsigned n) {
    assert(n);

    int got;
    if((got = write(fd, data, n)) < 0)
        sys_die(write, write_exact failed);
    if(got != n) 
        panic("expected a write of %d bytes, got %d\n", n, got);
    return n;
}
