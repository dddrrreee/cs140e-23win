// this is a boehm example without threads: can the compiler 
// re-order or remove a write?
void foo(int *p, int *q) {
    if(q != p) {
        *q = 1;
        *q = 3;
        *p = 2;
    }
    return;
}
