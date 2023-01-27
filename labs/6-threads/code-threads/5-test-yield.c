// run N threads that yield and explicitly call exit.
#include "test-header.h"

void trivial(void* arg) {
    trace("thread %d yielding\n", rpi_cur_thread()->tid);
    rpi_yield();
    trace("thread %d exiting\n", rpi_cur_thread()->tid);
    rpi_exit(0);
}

void notmain(void) {
    test_init();

    // make this > 1 to test
    int n = 10;
    for(int i = 0; i < n; i++)
        rpi_fork(trivial, (void*)i);
    rpi_thread_start();
    trace("SUCCESS\n");
}
