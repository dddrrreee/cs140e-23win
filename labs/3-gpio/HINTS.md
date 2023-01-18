# Hints for GPIO

## Output

1.  You write `GPFSELn` register (pages 91 and 92) to set up a pin as an
    output or input. You'll have to set GPIO 20 in GPFSEL2 to output.

2.  You'll turn it off and on by writing to the `GPSET0` and `GPCLR0`
    registers on page 95. We write to `GPSET0` to set a pin (turn it on)
    and write to `GPCLR0` to clear a pin (turn it off).

3.  The different `GPFSELn` registers handle group of 10, so you
    can divide the pin number to compute the right `GPFSEL` register.

4.  Be very careful to read the descriptions in the broadcom document to
    see when you are supposed to preserve old values or ignore them.
    If you don't ignore them when you should, you can write back
    indeterminate values, causing weird behavior. If you overwrite old
    values when you should not, the code in this assignment may work,
    but later when you use other pins, your code will reset them.

             // assume: we want to set the bits 7,8 in <x> to v and
             // leave everything else undisturbed.
             x &=  ~(0b11 << 7);   // clear the bits 7, 8  in x
             x |=   (v << 7);   // or in the new bits

5.  You will be using this code later! Make sure you test the code by
    rewiring your pi to use pins in each group.
