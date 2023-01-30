#include <stdarg.h>
#include <string.h>
#include "libunix.h"

// assumes there is enough space in <dst>
char *strcatf(char *dst, const char *fmt, ...) {
    char buf[4096];

    va_list args;
    va_start(args, fmt);
        if(vsnprintf(buf, sizeof buf, fmt, args) < 0)
            panic("overflowed buffer\n");
    va_end(args);
    strcat(dst, buf);
    return dst;
}

char *str2dupf(const char *src1, const char *fmt, ...) {
    char src2[4096];
    char dst[4096];

    va_list args;
    va_start(args, fmt);
        if(vsnprintf(src2, sizeof src2, fmt, args) >= sizeof src2)
            panic("overflowed buffer\n");
    va_end(args);

    if(snprintf(dst, sizeof dst, "%s%s", src1,src2) >= sizeof dst)
        panic("overflowed buffer\n");
    return strdup(dst);
}

// doesn't check destination size.
char *strcpyf(char *dst, const char *fmt, ...) {
    char buf[4096];

    va_list args;
    va_start(args, fmt);
        if(vsnprintf(buf, sizeof buf, fmt, args) < 0)
            panic("overflowed buffer\n");
    va_end(args);

    return strcpy(dst, buf);
}

char *vstrdupf(const char *fmt, va_list ap) {
    char buf[4096];
    if(vsnprintf(buf, sizeof buf, fmt, ap) >= sizeof buf)
        panic("overflowed buffer\n");
    return strdup(buf);
}

char *strdupf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
        char *str = vstrdupf(fmt, args);
    va_end(args);

    return str;
}
