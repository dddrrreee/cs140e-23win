#include "libos.h"

void notmain(void) {
    unsigned sum = 0;
    for(int i = 0; i < 8; i++) {
        int pid;

        if(!(pid = sys_fork())) {
            output("child pid=$pid\n");
            sys_exit(i);
        } else {
            output("parent pid=$pid, child pid=%d\n", pid);
            int res = sys_waitpid(pid);
            output("waitpid=%d\n", res);
            sum += res;
            if(res != i)
                panic("got=%d, expected=%d\n", res, i);
        }
    }
    output("SUCCESS: sum=%d\n", sum);
#if 0
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
