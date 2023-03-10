#include "rpi.h"
#include "pi-sd.h"
#include "libc/fast-hash32.h"

#if 0
    // old bzt driver
#   include "bzt-sd.h"
#else
    // zach's driver
#   include "emmc.h"
#endif

#include "libc/crc.h"

static int trace_p = 0;
static int init_p = 0;

int pi_sd_trace(int on_p) {
    int old = on_p;
    trace_p = on_p;
    return old;
}


// not sure if we should allow it to be called multiple times?
// just make it so the routines take in a handle.
int pi_sd_init(void) {
    if(sd_init() != SD_OK)
        panic("sd_init failed\n");
    init_p = 1;
    return 1;
}

int pi_sd_read(void *data, uint32_t lba, uint32_t nsec) {
  demand(init_p, "SD card not initialized!\n");
  int res;
  if((res = sd_readblock(lba, data, nsec)) != 512 * nsec)
    panic("could not read from sd card: result = %d\n", res);

  if(trace_p)
    trace("sd_read: lba=<%x>, cksum=%x\n", lba, fast_hash(data,nsec*512));
  return 1;
}

// allocate <nsec> worth of space, read in from SD card, return pointer.
// your kmalloc better work!
void *pi_sec_read(uint32_t lba, uint32_t nsec) {
  demand(init_p, "SD card not initialized!\n");
   // output("about to allocate %d\n", nsec * 512);
  uint8_t *data = kmalloc(nsec * 512);
  if(!pi_sd_read(data, lba, nsec))
    panic("could not read from sd card\n");
  return data;
}
#if 0
#endif

int pi_sd_write(void *data, uint32_t lba, uint32_t nsec) {
  demand(init_p, "SD card not initialized!\n");
  int res;
  if((res = sd_writeblock(data, lba, nsec)) != 512 * nsec)
    panic("could not write to sd card: result = %d\n", res);

  if(trace_p)
    trace("sd_write: lba=<%x>, cksum=%x\n", lba, our_crc32(data,nsec*512));
  return 1;
}
