#include <stdlib.h>
void opaque(void);
// what do you think gcc output is?
int main(void) {
    int *p = malloc(4);
    *p = 10;
    // printf("%d\n", *p);
    return 0;
}
