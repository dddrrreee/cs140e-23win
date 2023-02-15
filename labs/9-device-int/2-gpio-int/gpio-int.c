// engler, cs140 put your gpio-int implementations in here.
#include "rpi.h"

enum {
    GPIO_BASE = 0x20200000,
    GPIO_PAGE_SIZE = 0x1000,
    GPIO_GPFSEL0 = 0x00,
    GPIO_GPFSEL1 = 0x04,
    GPIO_GPFSEL2 = 0x08,
    GPIO_GPFSEL3 = 0x0C,
    GPIO_GPFSEL4 = 0x10,
    GPIO_GPFSEL5 = 0x14,
    GPIO_GPSET0 = 0x1C,
    GPIO_GPSET1 = 0x20,
    GPIO_GPCLR0 = 0x28,
    GPIO_GPCLR1 = 0x2C,
    GPIO_GPLEV0 = 0x34,
    GPIO_GPLEV1 = 0x38,
    GPIO_GPEDS0 = 0x40,
    GPIO_GPEDS1 = 0x44,
    GPIO_GPREN0 = 0x4C,
    GPIO_GPREN1 = 0x50,
    GPIO_GPFEN0 = 0x58,
    GPIO_GPFEN1 = 0x5C,
    GPIO_GPHEN0 = 0x64,
    GPIO_GPHEN1 = 0x68,
    GPIO_GPLEN0 = 0x70,
    GPIO_GPLEN1 = 0x74,
    GPIO_GPAREN0 = 0x7C,
    GPIO_GPAREN1 = 0x80,
    GPIO_GPAFEN0 = 0x88,
    GPIO_GPAFEN1 = 0x8C,
    GPIO_GPPUD = 0x94,
    GPIO_GPPUDCLK0 = 0x98,
    GPIO_GPPUDCLK1 = 0x9C,
};
// returns 1 if there is currently a GPIO_INT0 interrupt, 
// 0 otherwise.
//
// note: we can only get interrupts for <GPIO_INT0> since the
// (the other pins are inaccessible for external devices).
int gpio_has_interrupt(void) {
    DEV_VAL(;)
    //dev_val();
    return get32(GPIO_BASE + GPIO_GPEDS0) & (1 << GPIO_INT0);
    //todo("implement: is there a GPIO_INT0 interrupt?\n");
}

// p97 set to detect rising edge (0->1) on <pin>.
// as the broadcom doc states, it  detects by sampling based on the clock.
// it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
// *after* a 1 reading has been sampled twice, so there will be delay.
// if you want lower latency, you should us async rising edge (p99)
void gpio_int_rising_edge(unsigned pin) {
    // unsigned fsel = pin / 10, shift = (pin % 10) * 3;
    // put32(GPIO_BASE + GPIO_GPFEN0 + fsel*4, get32(GPIO_BASE + GPIO_GPFEN0 + fsel*4) | (1 << shift));
    // //todo("implement: detect rising edge\n");

    unsigned fsel = get32((void*)(GPIO_BASE + GPIO_GPFSEL0));
    fsel &= ~(7 << (pin * 3));
    put32((void*)(GPIO_BASE + GPIO_GPFSEL0), fsel);
    unsigned reg = get32((void*)(GPIO_BASE + GPIO_GPREN0));
    reg |= 1 << pin;
    put32((void*)(GPIO_BASE + GPIO_GPREN0), reg);
}

// p98: detect falling edge (1->0).  sampled using the system clock.  
// similarly to rising edge detection, it suppresses noise by looking for
// "100" --- i.e., is triggered after two readings of "0" and so the 
// interrupt is delayed two clock cycles.   if you want  lower latency,
// you should use async falling edge. (p99)
void gpio_int_falling_edge(unsigned pin) {
    unsigned fsel = get32((void*)(GPIO_BASE + GPIO_GPFSEL0));
    fsel &= ~(7 << (pin * 3));
    put32((void*)(GPIO_BASE + GPIO_GPFSEL0), fsel);
    unsigned reg = get32((void*)(GPIO_BASE + GPIO_GPFEN0));
    reg |= 1 << pin;
    put32((void*)(GPIO_BASE + GPIO_GPFEN0), reg);
    //todo("implement: detect falling edge\n");
}

// p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
// if you configure multiple events to lead to interrupts, you will have to 
// read the pin to determine which caused it.
int gpio_event_detected(unsigned pin) {
    //dev_val();
    DEV_VAL()
    return get32(GPIO_BASE + GPIO_GPEDS0) & (1 << pin);
    //todo("implement: is an event detected?\n");
}

// p96: have to write a 1 to the pin to clear the event.
void gpio_event_clear(unsigned pin) {
    put32(GPIO_BASE + GPIO_GPEDS0, 1 << pin);
    //todo("implement: clear event on <pin>\n");
}
