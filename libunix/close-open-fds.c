#include <sys/resource.h>
#include "libunix.h"

void close_open_fds_except(int fd) {
    struct rlimit l;

    // use getrlimit and <RLIMIT_NOFILE> to get the current max fd
    unimplemented();
}

// close all open descriptors other than stdin=0, stdout=1, stderr=2
// we could open with "close on exec" but (1) can't w/ ptty's, and (2)
// fragile, b/c it requires you never forget, ever.
void close_open_fds(void) {
    close_open_fds_except(-1);
}
