/* tests that implicit exit works */
#include "test-header.h"

static unsigned thread_count, thread_sum;

// trivial first thread: does not block, explicitly calls exit.
static void thread_code(void *arg) {
    unsigned *x = arg;

    // check tid
    rpi_thread_t *t = rpi_cur_thread();

	trace("in thread [%p], tid=%d with x=%d\n", t, t->tid, *x);
    demand(rpi_cur_thread()->tid == *x+1, 
                "expected %d, have %d\n", t->tid,*x+1);

	thread_count ++;
	thread_sum += *x;

    // no exit.
}

void notmain() {
    test_init();

    trace("about to test summing of 30 threads\n");

    // change this to increase the number of threads.
	int n = 30;
	thread_sum = thread_count = 0;

    unsigned sum = 0;
	for(int i = 0; i < n; i++)  {
        int *x = kmalloc(sizeof *x);
        sum += *x = i;
		rpi_fork(thread_code, x);
    }
	rpi_thread_start();

	// no more threads: check.
	trace("count = %d, sum=%d\n", thread_count, thread_sum);
	assert(thread_count == n);
	assert(thread_sum == sum);
    trace("SUCCESS!\n");
}

