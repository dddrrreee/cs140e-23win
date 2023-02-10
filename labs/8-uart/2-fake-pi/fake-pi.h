#ifndef __FAKE_PI_H__
#define __FAKE_PI_H__

#define random() "do not call random() directly"

// prototypes for our fake_random implementation

// must call this first: used to check that everyone
// has the same seed.
void fake_random_init(void);

// call to change seed.
void fake_random_seed(unsigned x);

// total number of random calls (can be used to debug
// your fake implementation).
unsigned fake_random_calls(void);

// call this to get a 32-bit pseudo-random number.
unsigned fake_random(void);

// call to exit: dump out some stats.
void fake_pi_exit(void);

#endif
