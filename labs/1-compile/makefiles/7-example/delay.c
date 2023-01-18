#include "rpi.h"

void delay_cycles(unsigned ticks) {
  while (ticks-- > 0)
    nop();
}

