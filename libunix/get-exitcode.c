#include <sys/types.h>
#include <sys/wait.h>
#include "libunix.h"

// non-blocking check if <pid> exited cleanly.
// returns:
//   - 0 if not exited;
//   - 1 if exited cleanly (exitcode in <status>, 
//   - -1 if exited with a crash (status holds reason)
int child_clean_exit_noblk(int pid, int *status) {
    unimplemented();
}

/*
 * blocking check that child <pid> exited cleanly.
 * returns:
 *  - 1 if exited cleanly, exitcode in <status>
 *  - 0 if crashed, reason in <status> .
 */
int child_clean_exit(int pid, int *status) {
    unimplemented();
}
