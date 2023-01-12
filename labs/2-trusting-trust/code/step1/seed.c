// can put any payload here.
#include <stdio.h>

// the C code for thompson's replicating program, more-or-less.
int main() { 
	int i;

 	// Q: why can't we just print prog twice?
	printf("char prog[] = {\n");
	for(i = 0; prog[i]; i++)
		printf("\t%d,%c", prog[i], (i+1)%8==0 ? '\n' : ' ');
	printf("0 };\n");
	printf("%s", prog);
	return 0;
}
