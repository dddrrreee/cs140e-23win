#include "rpi-interrupts.h"
//void software_interrupt_vector(unsigned pc) { INT_UNHANDLED("swi", pc); }
void int_vector(unsigned pc) { INT_UNHANDLED("interrupt", pc); }
