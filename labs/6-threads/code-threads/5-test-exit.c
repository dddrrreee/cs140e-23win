// run N trivial threads that explicitly call exit; no yield
#include "test-header.h"

void trivial(void* arg) {
    trace("trivial thread: arg=%d\n", (unsigned)arg);

    // manually call rpi_exit
    rpi_exit(0);
}

void notmain(void) {
    // make this > 1 to test
    int n = 10;
    for(int i = 0; i < n; i++)
        rpi_fork(trivial, (void*)i);
    rpi_thread_start();
    trace("SUCCESS\n");
}
