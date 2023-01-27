// Q: does push change sp before writing to it or after?
// Implement the <todo>
#include "rpi.h"

enum { push_val = 0xdeadbeef };

// should take a few lines: 
//   - push argument (in r0) onto the stack.
//   - call <after_push> with:
//      - the first argument: sp after you do the push.
//      - second argument: sp before you do the push.

// implement this assembly routine in <asm-checks.S>
void check_push_asm(uint32_t push_val);


// called with:
//   - <sp_after_push>: value of the <sp> after the push 
//   - <sp_before_push>: value of the <sp> before the push 
void after_push(uint32_t *sp_after_push, uint32_t *sp_before_push) {
    trace("sp_after=[%p] val=%x, sp_before=[%p] [val=%x]\n",
                    sp_after_push, 
                    *sp_after_push, 
                    sp_before_push, 
                    *sp_before_push);

    if(sp_after_push < sp_before_push)
        trace("stack grows down\n");
    else if(sp_after_push > sp_before_push)
        trace("stack grows up\n");
    else
        panic("impossible\n");

    // print one or the other depending:
    //      trace("wrote to stack after modifying sp\n");
    //      trace("wrote to stack before modifying sp\n");
    todo("implement this code: look at caller: <asm-check.S:check_push_asm>");

    // we can't return b/c didn't obey calling convention.
    clean_reboot();
}

void notmain() {
    printk("about to check asm push\n");
    check_push_asm(push_val);
    not_reached();
}
