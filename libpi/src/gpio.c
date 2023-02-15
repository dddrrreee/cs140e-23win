/*
 * Implement the following routines to set GPIO pins to input or output,
 * and to read (input) and write (output) them.
 *
 * DO NOT USE loads and stores directly: only use GET32 and PUT32
 * to read and write memory.  Use the minimal number of such calls.
 *
 * See rpi.h in this directory for the definitions.
 */
#include "rpi.h"

// see broadcomm documents for magic addresses.
enum {
    GPIO_BASE = 0x20200000,
    gpio_set0  = (GPIO_BASE + 0x1C),
    gpio_clr0  = (GPIO_BASE + 0x28),
    gpio_lev0  = (GPIO_BASE + 0x34)
};

void gpio_set_function(unsigned pin, gpio_func_t function)
{
    if(pin >= 32)
        return;
    if(function > 7)
        return;
    unsigned int mask = 0b111; 
    unsigned fsel = pin / 10;
    unsigned shift = (pin % 10) * 3; 
    unsigned int set = GET32(GPIO_BASE + 0x4 * fsel);
    set &= ~(mask << shift);
    set |= (function << shift); 
    PUT32(GPIO_BASE + 0x4 * fsel, set); 
}

//
// Part 1 implement gpio_set_on, gpio_set_off, gpio_set_output
//
// set <pin> to be an output pin.
//
// note: fsel0, fsel1, fsel2 are contiguous in memory, so you
// can (and should) use array calculations!
void gpio_set_output(unsigned pin) {
    if(pin >= 32)
        return;
    unsigned int mask = 0b111; //creates a mask value of 0b111 used to extract the 3 bits corresponding to the requested pin in the GPFSEL register.
    unsigned fsel = pin / 10;
    //(Each register controls 10 pins)so dividing the pin number by 10 gives the correct register number.
    unsigned shift = (pin % 10) * 3; //bit shift calculation
    //(Each pin is represented by 3 bits in the GPFSEL register) so multiplying the remainder of pin number divided by 10 by 3 gives the correct bit shift.
    unsigned int set = GET32(GPIO_BASE + 0x4 * fsel);
    //reads the current value of the GPFSEL register corresponding to the requested pin into the variable "set" using the GET32 macro.
    set &= ~(mask << shift);
    //clears the 3 bits corresponding to the requested pin in the GPFSEL register by ANDing the current value of the GPFSEL register with the complement of the mask shifted by the calculated bit shift.
    set |= (001 << shift); 
    //sets the 3 bits corresponding to the pin in the GPFSEL register to 001, which is the binary representation of the output mode.
    PUT32(GPIO_BASE + 0x4 * fsel, set); //writes the updated value of the GPFSEL register back to memory using the PUT32 macro.

   //volatile unsigned *gpset0 = (void*)0x2020001C;
  // implement this
  // use <gpio_fsel0>
}

// set GPIO <pin> on.
void gpio_set_on(unsigned pin) {
    if(pin >= 32)
        return;
    PUT32(gpio_set0, 1 << pin); // This line sets the pin high by writing a 1 to the corresponding bit in the GPSET0 register. The value of 1 is left-shifted by the pin number so that the 1 is written to the correct bit.
  // implement this
  // use <gpio_set0>
}

// set GPIO <pin> off
void gpio_set_off(unsigned pin) {
    if(pin >= 32)
        return;
    PUT32(gpio_clr0, 1 << pin);
  // implement this
  // use <gpio_clr0>
}

// set <pin> to <v> (v \in {0,1})
void gpio_write(unsigned pin, unsigned v) {
    if(pin >= 32)
        return;
    if(v)
        gpio_set_on(pin);
    else
        gpio_set_off(pin);
}

//
// Part 2: implement gpio_set_input and gpio_read
//

// set <pin> to input.
void gpio_set_input(unsigned pin) {
    if(pin >= 32)
        return;
    unsigned fsel = pin / 10;
    unsigned shift = (pin % 10) * 3;
    unsigned int mask = 0b111;
    unsigned int set = GET32(GPIO_BASE + 0x4 * fsel);
    set &= ~(mask << shift);
    PUT32(GPIO_BASE + 0x4 * fsel, set);
}
  // implement.


// return the value of <pin>
int gpio_read(unsigned pin) {
  //unsigned v = 0;
  //return v;
   if(pin >= 32)
        return -1;
    unsigned int mask = 1 << pin; //creates a variable "mask" with a binary value of "1" shifted left by the pin number. 
    return DEV_VAL32((GET32(gpio_lev0) & mask) >> pin);//reads the state of the specified pin by ANDing the GPLEV0 register with the mask, which extracts the value of the specified pin from the GPLEV0 register. 
  // implement.
  
}
