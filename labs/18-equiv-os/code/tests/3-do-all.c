#include "equiv-os.h"

#include "user-progs/byte-array-prog-vec.h"
#include "user-progs/byte-array-prog-exit.h"
#include "user-progs/byte-array-prog-alloc.h"
#include "user-progs/byte-array-prog-fork.h"
#include "user-progs/byte-array-prog-fork-delay.h"

void init(pctx_t *ctx) {
    if(0) {
        sys_exec(ctx, &prog_fork_delay, 0x77cc8e24); //  R*2;
        sys_exec(ctx, &prog_fork_delay, 0x77cc8e24); //  R*2;
        sys_exec(ctx, &prog_fork_delay, 0x77cc8e24); //  R*2;
    }

    sys_exec(ctx, &prog_fork_delay, 0x77cc8e24);
    sys_exec(ctx, &prog_fork, 0x48eb8af8);
    for(int i = 0; i < 3; i++) {
        sys_exec(ctx, &prog_vec, 0x2e18f06c)->respawn = R;
        sys_exec(ctx, &prog_exit, 0xcb436bdd)->respawn = 2*R;
        sys_exec(ctx, &prog_alloc, 0xf8b44b9b)->respawn = 2*R;
    }
}
