#include "rpi.h"
#include "mbox.h"

uint32_t rpi_clock_hz_set(uint32_t clock, uint32_t hz) {
    /*
    Tag: 0x00038002
    Request:
    Length: 12
    Value:
    u32: clock id
    u32: rate (in Hz)
    u32: skip setting turbo
    Response:
    Length: 8
    Value:
    u32: clock id
    u32: rate (in Hz)
    */
    static volatile unsigned *u = 0;

    if(!u)
        u = kmalloc_aligned(9*4,16);
    memset((void*)u, 0, 9*4);

    u[0] = 9*4;   // total size in bytes.
    u[1] = 0;   // always 0 when sending.
    // serial tag
    u[2] = 0x00038002;
    u[3] = 12;   // data buffer size
    u[4] = 0;   // request code
    u[5] = clock;   // clock id
    u[6] = hz;   // clock hz
    u[7] = 0;   // skip turbo
    u[8] = 0;   // end tag

    mbox_send(MBOX_CH, u);
    return u[6];
}
