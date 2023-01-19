// trivial program to show how ld function wrapping works.
#include "rpi.h"
#include "foo.h"

// gross: ld creates these but we have to define by hand.  be very
// careful!  you could do some macro tricks, but we keep it simple.
unsigned __wrap_foo(unsigned arg0, unsigned arg1);
unsigned __real_foo(unsigned arg0, unsigned arg1);

// wrapper inserted by ld (see makefile)
unsigned __wrap_foo(unsigned arg0, unsigned arg1) {
    printk("in wrap foo(%u,%u)\n", arg0,arg1);

    // show that we can mess with the arguments and forward them.
    arg0++;
    arg1++;
    printk("calling real foo(%u,%u)\n", arg0,arg1);
    unsigned ret = __real_foo(arg0,arg1);
    printk("returning from wrap_foo\n");
    return ret;
}

void notmain(void) {
    unsigned a0 = 1, a1 = 2;
    printk("about to call foo(%d,%d)\n", a0,a1);
    unsigned ret = foo(a0,a1);
    printk("returned  %u\n", ret);
    clean_reboot();
}
