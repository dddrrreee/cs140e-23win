#include "equiv-os.h"
#include "user-progs/byte-array-prog-exit.h"

// do a single exit.
void init(pctx_t *ctx) {
    sys_exec(ctx, &prog_exit, 0xcb436bdd)->respawn = 2*R;
}
