

/*
00000000 <mul>:
   0:	e1a03000 	mov	r3, r0
   4:	e3a0c000 	mov	ip, #0
   8:	e0000192 	mul	r0, r2, r1
   c:	e583c000 	str	ip, [r3]
  10:	e12fff1e 	bx	lr
*/

#define gcc_mb() asm volatile ("" : : : "memory")
// simple routine to time multiplication.
int mul(int *time, int a, int b) {
	unsigned start = *(volatile unsigned *) 0x20003004;

    asm volatile ("" : : : "memory");
	int c = a * b;
    asm volatile ("" : : : "memory");

    unsigned end = *(volatile unsigned *) 0x20003004;

	*time = end - start;
	return c;
}

void x(volatile int * xx) {
    *xx;
    *xx;
    *xx;
}
