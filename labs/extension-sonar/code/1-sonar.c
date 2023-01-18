// simple driver for hc-sr04
#include "rpi.h"
#include "hc-sr04.h"

// use this timeout(in usec) so everyone is consistent.
enum { timeout = 55000, echo_pin = 21, trigger_pin = 20 };

void notmain(void) {
	printk("starting sonar: trigger=%d, echo=%d!\n", 
            trigger_pin, echo_pin);
    hc_sr04_t h = hc_sr04_init(trigger_pin, echo_pin);
	printk("sonar ready!\n");

    for(int dist, ntimeout = 0, i = 0; i < 10; i++) {
        int dist;
        // read until no timeout.
        while((dist = hc_sr04_get_distance(h, timeout)) < 0)
            printk("timeout!\n");
        printk("distance = %d inches\n", dist);
        // wait a second
        delay_ms(1000);
    }
	printk("stopping sonar !\n");
    clean_reboot();
}
