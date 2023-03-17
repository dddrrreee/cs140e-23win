#include "equiv-os.h"
#include "user-progs/byte-array-prog-fork-delay.h"
void init(pctx_t *ctx) {
    sys_exec(ctx, &prog_fork_delay, 0x77cc8e24); //  R*2;
}
