// a local cstart so we can nuke the UART config before notmain
// runs --- otherwise the fact that the bootloader leaves it in 
// an initialized state can hide many mistakes.
#include "rpi.h"
#include "cycle-count.h"
#include "memmap.h"

void uart_destroy(void) {
    // hack to make sure aux is off.
    dev_barrier();
    PUT32(0x20215004, 1);
    dev_barrier();
    dev_barrier();
    gpio_set_function(GPIO_TX, 0);
    gpio_set_function(GPIO_RX, 0);
    dev_barrier();
    uint32_t addr = 0x20215040;
    for(unsigned off = 0; off < 11; off++)
        PUT32(addr+off*4, 0);
    dev_barrier();
    PUT32(0x20215004, 0);
    dev_barrier();
}

// only change is that we don't call uart_init
void _cstart() {
    // zero out the bss
    uint32_t * bss = __bss_start__;
    uint32_t * bss_end = __bss_end__;
    while( bss < bss_end )
        *bss++ = 0;

    // have to initialize the cycle counter or it returns 0.
    // i don't think any downside to doing here.
    cycle_cnt_init();
    uart_destroy();
    notmain(); 
	clean_reboot();
}
