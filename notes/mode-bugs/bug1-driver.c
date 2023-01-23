#include "rpi.h"

void switch_to_system_bug(void);
uint32_t cpsr_get(void);

void notmain(void) {
    uint32_t cpsr = cpsr_get();
    printk("cpsr = <%b>\n", cpsr);

    uint32_t mode = cpsr&0b11111;
    printk("     mode = <%b>\n", mode);
    assert(mode == SUPER_MODE);

    printk("    interrupt = <%s>\n", 
                ((cpsr >> 7)&1) ? "off" : "on");

    switch_to_system_bug();
    printk("switched to system\n");
}
