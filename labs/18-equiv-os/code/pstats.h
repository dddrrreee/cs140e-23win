#ifndef __PSTATS_H__
#define __PSTATS_H__
// some trivial statistics counting how many forks, how many instructions,
// how many context switches.
//  - these should be the same from run to run other than the time.
#include "pretty-time.h"

typedef struct {
    uint32_t start_usec;

    unsigned n_forked;
    unsigned n_checked;
    unsigned n_inst;
    unsigned n_switches;
    unsigned n_exit;
    unsigned n_respawn;
    unsigned n_vm_off_on;

    // print out the stats every <n> single step exceptions.
    unsigned print_every_n;
} pstats_t;

static inline pstats_t pstats_mk(void) {
    return (pstats_t) {};
}
static inline void pstats_start(pstats_t *ps) {
    ps->start_usec = timer_get_usec();
}

// number of instructions executed.
static void pstats_inst_inc(pstats_t *ps, proc_t *p) {
    ps->n_inst++;
}
// number of processes forked.
static inline void pstats_forked_inc(pstats_t *ps, proc_t *p) {
    ps->n_forked++;
}
// number of processes checked
static inline void pstats_checked_inc(pstats_t *ps, proc_t *p) {
    ps->n_checked++;
}
static inline void pstats_respawn_inc(pstats_t *ps, proc_t *p) {
    ps->n_respawn++;
}

// number of process switches done.
static inline void pstats_switch_inc(pstats_t *ps, proc_t *p) {
    ps->n_switches++;
}
// number of processes exited (should be same as forked)
static inline void pstats_exit_inc(pstats_t *ps, proc_t *p) {
    ps->n_exit++;
}
static inline void pstats_vm_off_on_inc(pstats_t *ps, proc_t *p) {
    ps->n_vm_off_on++;
}

static inline void 
pstats_print(const char *msg, pstats_t *ps, int done_p) {
    output("\n");
    trace("----------------------------------------------------------\n");
    trace("%s:  \n", msg);
    trace("\ttotal time = [", msg);
    pretty_print_usec(ps->start_usec);
    output("]\n");

    trace("\tstats for <%s>:\n", msg);
    if(ps->n_forked)
        trace("\t   n forked=%d\n", ps->n_forked);
    if(ps->n_respawn)
        trace("\t   n respawn=%d\n", ps->n_respawn);
    if(ps->n_vm_off_on)
        trace("\t   n turned vm off/on=%d\n", ps->n_vm_off_on);

    if(ps->n_exit) {
        if(!done_p)
            trace("\t   n exits=%d\n", ps->n_exit);
#if 0
        else if(ps->n_exit != ps->n_forked)
            panic("\t   n exits=%d\n", ps->n_exit);
#endif
    }

    if(ps->n_checked) {
        if(!done_p) 
            trace("\t   n checked=%d\n", ps->n_respawn);
#if 0
        else if(ps->n_checked != ps->n_forked)
            panic("n hash checks=%d\n", ps->n_checked);
#endif
    }
    if(ps->n_switches)
        trace("\t   n switches=%d\n", ps->n_switches);
    if(ps->n_inst)
        trace("\t   n inst run=%d\n", ps->n_inst);
    trace("----------------------------------------------------------\n");
}
#endif
