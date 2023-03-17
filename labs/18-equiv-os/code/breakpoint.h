#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__


// disable mismatch
void brkpt_mismatch_stop(void);

// enable mismatch
void brkpt_mismatch_start(void);

// set breakpoint on <addr>
void brkpt_mismatch_set(uint32_t addr);

// is it a breakpoint fault?
int brkpt_fault_p(void);

#endif
