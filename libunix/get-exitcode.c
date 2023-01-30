#include "libunix.h"

// non-blocking check if <pid> exited cleanly.
// returns:
//   - 0 if not exited;
//   - 1 if exited cleanly (exitcode in <status>, 
//   - -1 if exited with a crash (status holds reason)
int child_clean_exit_noblk(int pid, int *status) {
    unimplemented();
}
