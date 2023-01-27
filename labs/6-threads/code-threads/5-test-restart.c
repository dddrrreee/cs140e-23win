// run the threads package N times: make sure you can 
// invoke multiple times.
#include "test-header.h"

static void thread_code(void *arg) {
    unsigned *x = arg;

    // not sure if we should reset the tid across runs?
    rpi_thread_t *t = rpi_cur_thread();

	trace("in thread [%p], tid=%d with x=%x\n", t, t->tid, *x);
    assert(*x == 0xdeadbeef);
//    assert(t->tid == 1 && *x == 0xdeadbeef);

    trace("success: got to the first thread\n");
    rpi_exit(0);
}

void notmain() {
    test_init();

    // make sure starting threads with no threads works.
    for(int i = 0; i < 10; i++) {
        trace("i=%d: about to start threads package wth nothing\n", i);
	    rpi_thread_start();
    }

    // run the threads package N times.
    unsigned x = 0xdeadbeef;
    for(int i = 0; i < 10; i++) {
        trace("i=%d: about to fork and run one thread\n", i);
        
	    rpi_fork(thread_code, &x);
	    rpi_thread_start();
    }
}
