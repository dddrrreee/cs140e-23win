// -mcpu=arm1176jzf-s -mtune=arm1176jzf-s
//   mem difference of heap writes would find this.
//   good way to find compiler bugs.
#include "rpi.h"
#include <stddef.h>

typedef struct {
    uint8_t spi_chip, ce_pin;
} nrf_conf_t;

typedef struct nrf {
    uint8_t enabled_p; 
    nrf_conf_t config;
} nrf_t;

nrf_t * struct_copy(nrf_conf_t c);

void notmain(void) {
    nrf_conf_t c = { .spi_chip = 1, };
    
    nrf_t *n = struct_copy(c);
    output("spi=%d  ce=%d off=%d\n", n->config.spi_chip, n->config.ce_pin,
        offsetof(nrf_t, config));

    if(c.spi_chip != n->config.spi_chip)
        panic("not equal!! c.spi_chip = %d.   n->config.spi_chip = %d.\n",
            c.spi_chip, n->config.spi_chip);

    assert(memcmp(&n->config, &c, sizeof c) == 0);
}

/*
  bug.  does not compute struct offsets correctly.

00008038 <struct_copy>:
    8038:   e52de004    push    {lr}        ; (str lr, [sp, #-4]!)
    803c:   e24dd00c    sub sp, sp, #12
    8040:   e1cd00b4    strh    r0, [sp, #4]
    8044:   e3a00003    mov r0, #3
    8048:   eb0002a3    bl  8adc <kmalloc>
    804c:   e1dd30b4    ldrh    r3, [sp, #4]
    8050:   e1c030b1    strh    r3, [r0, #1] <---- bug!
    8054:   e28dd00c    add sp, sp, #12
    8058:   e49df004    pop {pc}        ; (ldr pc, [sp], #4)
*/
nrf_t *
struct_copy(nrf_conf_t c) {
    nrf_t *n = kmalloc(sizeof *n);
    n->config = c;
    return n;
}
