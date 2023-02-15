#include "rpi.h"

void asm_not_reached_helper(uint32_t pc, const char *msg) {
    output("ERROR:%s: reached supposedly unreachable ASM code\n");
    output("\tUse pc=%x and .list file to see what happened!\n", pc);
    // should do an error.
    clean_reboot();
}

void asm_not_implemented_helper(uint32_t pc, const char *msg) {
    output("ERROR:%s: you must implement this ASM code\n");
    output("\tUse pc=%x and .list file to see what happened!\n", pc);
    // should do an error.
    clean_reboot();
}

// the asm code does some hacks to pass in the file and lineno
void asm_todo_helper(uint32_t pc, const char *todo_msg) {
    output("----------------------------------------------------\n");
    output("ERROR:TODO: must implement this ASM code for lab:\n");
    output("   %s\n", todo_msg);
    // output("   Use pc=%x and .list file to see what happened!\n", pc);
    // should do an error.
    clean_reboot();
}

void asm_bad_exception_helper(uint32_t pc, const char *msg) {
    output("ERROR:BAD EXCEPTION:%s: pc=%x\n", msg, pc);
    clean_reboot();
}
