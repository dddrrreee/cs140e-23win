#include "rpi.h"
#include "cycle-count.h"
#include "memmap.h"

void _cstart() {
    // extern int __bss_start__, __bss_end__;
	void notmain();

    custom_loader();

    // zero out the bss
    uint32_t * bss = __bss_start__;
    uint32_t * bss_end = __bss_end__;
 
    while( bss < bss_end )
        *bss++ = 0;

    uart_init();

    // have to initialize the cycle counter or it returns 0.
    // i don't think any downside to doing here.
    cycle_cnt_init();

    notmain(); 
	clean_reboot();
}
