#ifndef __TIME_PRINT_H__
#define __TIME_PRINT_H__

static inline unsigned time_to_sec(unsigned tot) {
    return tot / (1000*1000);
}
static inline unsigned time_to_msec(unsigned tot) {
    unsigned ms =  tot % (1000*1000);
    return ms / 1000;
}
static inline unsigned time_to_usec(unsigned tot) {
    return tot % 1000;
}

static void time_print(const char *msg, uint32_t start_usec) {
    unsigned t = timer_get_usec() - start_usec;
    unsigned sec = time_to_sec(t);
    unsigned ms = time_to_msec(t);
    unsigned usec = time_to_usec(t);

    output("%s: [total time=", msg);
    if(sec)
        output("%dsec:", sec);
    if(ms)
        output("%dms:", ms);
    if(usec)
        output("%dusec:", usec);
    output("]\n");
}

#endif
