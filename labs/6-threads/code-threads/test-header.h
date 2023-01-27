#ifndef __TEST_INIT_H__
#define __TEST_INIT_H__

#include "rpi.h"
#include "rpi-thread.h"

static void inline test_init(void) {
    unsigned oneMB = 1024*1024;
    kmalloc_init_set_start((void*)oneMB, oneMB);
}

#endif
