// yield with an empty run queue.

// forks one thread and does a yield: makes sure that yield can 
// work with an empty run queue.
#include "test-header.h"

void trivial(void* arg) {
    trace("thread tid=%d yielding\n", rpi_cur_thread()->tid);
    rpi_yield();
    trace("thread tid=%d exiting\n", rpi_cur_thread()->tid);

    // we manually call rpi_exit so works with earlier parts of labs
    rpi_exit(0);
}

void notmain(void) {
    test_init();

    output("running one thread, yielding with an empty queue\n");
    rpi_fork(trivial, 0);
    rpi_thread_start();
    trace("SUCCESS\n");
}
