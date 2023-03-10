#ifndef __RPI_PI_SD_H__
#define __RPI_PI_SD_H__

#define NBYTES_PER_SECTOR 512

// initialize the PI SD driver
int pi_sd_init(void);

// read `nsec` sectors of the SD card starting at `lba` into a buffer
int pi_sd_read(void *data, uint32_t lba, uint32_t nsec);

// allocate <nsec> worth of space, read in from SD card, return pointer.
// your kmalloc better work!
void *pi_sec_read(uint32_t lba, uint32_t nsec);

// write `data` to `nsec` sectors of the SD card starting at `lba`
int pi_sd_write(void *data, uint32_t lba, uint32_t nsec);

#endif
