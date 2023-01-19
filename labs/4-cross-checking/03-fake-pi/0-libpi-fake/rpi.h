#ifndef __RPI_H__
#define __RPI_H__
// simplest way to rewrap --- just define replacements in header --- don't
// even need .c
//
// easy, but two downsides: 
//   1. we are polluting the pi program namespace with system includes.
//   2. we aren't using the raw rpi.h so can get different behavior.
#include <stdio.h>
#include <stdlib.h>

// redefine printk as fprintf using variadic macros.
#define printk(args...) fprintf(stderr, args)

static void inline clean_reboot(void) {
    printk("DONE!!!\n");
    exit(0);
}

void notmain(void);
#endif
