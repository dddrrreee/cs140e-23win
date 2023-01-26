// very cool.
#include "rpi.h"

// have backtrace.

void __cyg_profile_func_enter (void *this_fn, void *call_site) 
  __attribute__((no_instrument_function));
void __cyg_profile_func_exit  (void *this_fn, void *call_site) 
  __attribute__((no_instrument_function));

void __cyg_profile_func_enter (void *this_fn, void *call_site) 
{
    static int in_enter;

    if(in_enter)
        return;

    in_enter = 1;
    printk("tracing %x\n", this_fn);
    in_enter = 0;
}

void __cyg_profile_func_exit  (void *this_fn, void *call_site) 
{
    return;
}
