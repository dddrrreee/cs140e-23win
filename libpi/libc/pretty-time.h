#ifndef __PRETTY_TIME_H__
#define __PRETTY_TIME_H__

// these calculations can wrap: should switch to the uint64
// timeout code.  or get the code from matthew?

// how many seconds in <tot_usec>
static inline uint32_t usec_to_sec(uint32_t tot_usec) {
    return tot_usec / (1000*1000);
}

// how many msec remain in <tot_usec> after you remove seconds.
static inline uint32_t usec_to_msec(uint32_t tot_usec) {
    unsigned ms =  tot_usec % (1000*1000);
    return ms / 1000;
}

// have many usec remain in <tot_usec> after you remove sec/ms.
static inline uint32_t usec_to_usec(uint32_t tot_usec) {
    return tot_usec % 1000;
}

typedef struct {
    uint32_t sec;
    uint32_t msec;
    uint32_t usec;
} pretty_time_t;

static inline pretty_time_t 
pretty_time_mk(uint32_t start_usec) {
    uint32_t tot_usec = timer_get_usec() - start_usec;
    return (pretty_time_t) {
        .sec = usec_to_sec(tot_usec),
        .msec = usec_to_msec(tot_usec),
        .usec = usec_to_usec(tot_usec)
    };
}

static inline void 
pretty_time_print(pretty_time_t t) {
    if(t.sec)
        output("%dsec:", t.sec);
    if(t.msec)
        output("%dms:", t.msec);
    if(t.usec)
        output("%dusec", t.usec);
}

static inline void 
pretty_print_usec(uint32_t start_usec) {
    pretty_time_print(pretty_time_mk(start_usec));
}

#endif
