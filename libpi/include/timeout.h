#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__
#include "rpi.h"

// problem: we want to time-out connections, but this won't
// work with raw rpi usec timers since we might want to wait
// longer than wrap around can happen.
// 
// each time we want to wait, we record the current state of time.
//
// NOTE: you must keep checking (calling <timeout_get_usec>) more frequently
// than a wrap-around can happen (4billion cycles) otherwise we will lose
// some ticks.
//
// alternatively: we could have interrupt code with timing, etc and explicitly
// track time like a real OS.  however, this obviously makes things significatly
// more complex.
typedef struct {
    uint32_t time_last;
    uint64_t time_usecs;
} timeout_t;

// call this first.
static timeout_t timeout_start(void) {
    return (timeout_t) { .time_usecs = 0, .time_last = timer_get_usec() };
}

// return the number of usecs since we started tracking time.
// don't need to use this directly.
static inline uint64_t timeout_get_usec(timeout_t *t) {
    uint32_t now = timer_get_usec();
    t->time_usecs += (now - t->time_last);
    t->time_last = now;
    return t->time_usecs;
}

#if 0
static inline int timeout_null(timeout_t *t) {
    return t->time_last == 0 && t->time_usecs == 0;
}
#endif

static inline int timeout_usec(timeout_t *t, uint64_t max_usec) {
    return timeout_get_usec(t) >= max_usec;
}
static inline int timeout_msec(timeout_t *t, uint64_t max_msec) {
    return timeout_usec(t, max_msec*1000);
}
static inline int timeout_sec(timeout_t *t, uint64_t max_sec) {
    return timeout_msec(t, max_sec*1000);
}
#endif
