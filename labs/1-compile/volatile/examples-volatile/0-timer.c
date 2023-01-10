



// simple routine to time multiplication.
int mul(int *time, int a, int b) {
	volatile unsigned start = *(unsigned *) 0x20003004;
	int c = a * b;
	volatile unsigned end = *(unsigned *) 0x20003004;

	*time = end - start;
	return c;
}
