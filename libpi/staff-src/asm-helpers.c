#include "rpi.h"

void asm_not_reached_helper(uint32_t pc) {
    output("ERROR: pc=%x: reached supposedly unreachable assembly code\n", pc);
    output("\tExamine the .list file to see what happened!\n");
    // should do an error.
    clean_reboot();
}

void asm_not_implemented_helper(uint32_t pc) {
    output("ERROR: pc=%x: reached unimplemented assembly code\n", pc);
    output("\tExamine the .list file to see what happened!\n");
    // should do an error.
    clean_reboot();
}
