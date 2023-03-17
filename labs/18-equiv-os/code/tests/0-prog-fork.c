#include "equiv-os.h"

#include "user-progs/byte-array-prog-fork.h"
void init(pctx_t *ctx) {
    sys_exec(ctx, &prog_fork, 0x48eb8af8);
}
