#include "equiv-os.h"
#include "user-progs/byte-array-prog-alloc.h"

void init(pctx_t *ctx) {
    sys_exec(ctx, &prog_alloc, 0xf8b44b9b)->respawn = 2*R;
}
