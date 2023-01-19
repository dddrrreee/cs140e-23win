#include "rpi.h"

// print out a special message so bootloader exits
void clean_reboot(void) {
    putk("DONE!!!\n");
    uart_flush_tx();
    delay_ms(10);       // (hopefully) enough time for message to get flushed.
    rpi_reboot();
}
