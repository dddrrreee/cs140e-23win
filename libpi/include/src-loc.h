#ifndef __SRC_LOC_H__
#define __SRC_LOC_H__

typedef struct {
    // XXX: add backtrace?
    const char *file;
    const char *func;
    unsigned lineno;
} src_loc_t; 

static inline 
src_loc_t src_loc_mk(const char *file, const char *func, unsigned lineno) {
    return (src_loc_t) { .file = file, .func = func, .lineno = lineno };
}
#define SRC_LOC_MK() src_loc_mk(__FILE__, __FUNCTION__, __LINE__)

#define loc_panic(l, args...) do {                              \
    output("LOC_PANIC: %s:%s:%d:", (l).file, (l).func, (l).lineno);        \
    output(args);                                               \
    clean_reboot();                                                   \
} while(0)

#define loc_debug(l, args...) do {                              \
    output("%s:%s:%d:", (l).file, (l).func, (l).lineno);        \
    output(args);                                                \
} while(0)

#endif
