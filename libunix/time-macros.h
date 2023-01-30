#ifndef __TIME_MACROS_H__
#define __TIME_MACROS_H__

#include "libunix.h"

// some helper macros to make measuring the cycle count of
// different operations easier.
#define TIME_CYC(_fn) ({                \
    time_usec_t _s = time_get_usec();     \
    _fn;                                \
    time_usec_t _e = time_get_usec();     \
    ((_e - _s) - 0);                    \
})

#define TIME_CYC_10(_fn) ({                     \
    time_usec_t _s = time_get_usec();             \
    _fn; _fn; _fn; _fn; _fn;                    \
    _fn; _fn; _fn; _fn; _fn;                    \
    time_usec_t _e = time_get_usec();             \
    ((_e - _s)-0);                              \
})

#define TIME_CYC_PRINT(_msg, _fn) do {                          \
    time_usec_t _t = TIME_CYC(_fn);                                \
    printk("%s: %ld microsecs <%s>\n", _msg, _t, _XSTRING(_fn));    \
} while(0)

#define TIME_CYC_PRINT10(_msg, _fn) do {                            \
    time_usec_t _t = TIME_CYC_10(_fn);                                 \
    printk("%s: %ld microsecs (x10) <%s>\n", _msg, _t, _XSTRING(_fn));  \
} while(0)

#endif
