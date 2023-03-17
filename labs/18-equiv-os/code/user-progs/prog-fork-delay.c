#include "libos.h"

void delay(unsigned ticks) {
    while(ticks-- > 0)
        asm volatile("nop");
}

enum { N = 8 };
void notmain(void) {

    for(int pid = 1; pid <= N; pid++) {
        if(!sys_fork()) {
            output("child pid=$pid\n");
            delay(100);
            sys_exit(pid);
        }
    }

    unsigned sum = 0;
    for(int pid = 1; pid <= N; pid++) {
        output("parent pid=$pid, waiting on child pid=%d\n", pid);

        int res = sys_waitpid(pid);
        output("USER: waitpid returned=%d [pid=%d]\n", res, pid);
        sum += res;
        if(res != pid)
            panic("got=%d, expected=%d\n", res, pid);
    }
    output("SUCCESS: sum=%d\n", sum);
    output("SUCCESS: sum=%d\n", sum);
    output("SUCCESS: sum=%d\n", sum);
    output("SUCCESS: sum=%d\n", sum);
    output("SUCCESS: sum=%d\n", sum);
    output("SUCCESS: sum=%d\n", sum);
#if 0
    assert(sum == 36);
#if 0
    sys_fork();
#else
    int pid;
    if(!(pid = sys_fork())) {
        output("child pid=$pid\n");
        sys_exit(2);
    } else {
        output("parent pid=$pid, child pid=%d\n", pid);
        int res = sys_waitpid(pid);
        output("waitpid=%d\n");
        if(res != 2)
            panic("got=%d, expected 2\n", res);
    }
#endif
#endif
    sys_exit(0);
}
