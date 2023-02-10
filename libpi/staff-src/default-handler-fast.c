#include "rpi-interrupts.h"
void fast_interrupt_vector(unsigned pc) { INT_UNHANDLED("fast", pc); }
void fiq_vector(unsigned pc) { INT_UNHANDLED("fast", pc); }
