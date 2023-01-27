/* 
 * simplest test: run a single thread.   should be the easiest test
 * to debug.  don't need context switch, yield, exit.
 */
#include "test-header.h"

// trivial first thread: does not block, returns.
// makes sure you pass in the right argument.
static void thread_code(void *arg) {
    unsigned *x = arg;

    // check tid
    rpi_thread_t *t = rpi_cur_thread();

	trace("in thread [addr=%p], tid=%d with x=%x\n", t, t->tid, *x);
    demand(rpi_cur_thread()->tid == *x, 
                "expected %d, have %d\n", t->tid,*x+1);

    *x += 1;
}

void notmain() {
    test_init();

    trace("about to run 1 thread\n");

    int x = 1;
	rpi_fork(thread_code, &x);
	rpi_thread_start();
    assert(x == 2);

    trace("SUCCESS!\n");
}
