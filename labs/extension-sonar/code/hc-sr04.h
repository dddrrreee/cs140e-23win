#ifndef __HC_SR04_H__
#define __HC_SR04_H__

// simple interface for the hc-sr04
#include "rpi.h"

// common pattern: devices usually have various bits of internal state.  wrap
// it up in a structure.  we return a copy to we don't have to malloc/free it.
typedef struct {
    unsigned trigger, echo;
} hc_sr04_t;

// initialize:
//  1. setup the <trigger> and <echo> GPIO pins.
// 	2. init the HC-SR04 (pay attention to time delays here)
hc_sr04_t hc_sr04_init(unsigned trigger, unsigned echo);

// return distance or -1 if you got a timeout.
int hc_sr04_get_distance(hc_sr04_t h, unsigned timeout);

#endif
