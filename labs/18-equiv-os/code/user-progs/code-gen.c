#include "libunix.h"

int main(int argc, char *argv[]) {
    if(argc != 2)
        panic("expected argc=2, have %d\n", argc);

    // lame canonicalization of name.
    char var[1024], *name = argv[1];
    unsigned i;
    for(i = 0; name[i]; i++) {
        if(name[i] == '.')
            break;
        else if(name[i] == '-')
            var[i] = '_';
        else
            var[i] = name[i];
    }
    var[i] = 0;
        
    unsigned nbytes = 0;
    uint8_t *code = read_file(&nbytes, argv[1]);

    printf("#include \"libos-prog.h\"\n");
    printf("struct prog %s = {\n", var);
    printf("    .name = \"%s\",\n", argv[1]);
    printf("    .nbytes = %d,\n", nbytes);
    printf("    .code = {");
    for(unsigned i = 0; i < nbytes; i++) {
        if(i % 8 == 0)
            printf("\n\t\t");
        printf("0x%x,\t", code[i]);
    }
    printf("    }\n");
    printf("};\n");
    return 0;
}
