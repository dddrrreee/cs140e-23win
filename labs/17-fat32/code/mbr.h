#ifndef __RPI_MBR_H__
#define __RPI_MBR_H__
#include "pi-sd.h"
#include "mbr-helpers.h"

// Load the MBR from the disk and verify it.
mbr_t *mbr_read();

#endif
