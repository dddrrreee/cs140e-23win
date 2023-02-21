/* simplifying wrapper for random.c */
#ifndef __PI_RANDOM_H__
#define __PI_RANDOM_H__

// get a random number
uint32_t pi_random(void);

// re-initialize with a seed.
void pi_random_seed(uint32_t x);

#endif

