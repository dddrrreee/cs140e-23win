/*  this script tells the linker how to layout our pi executable's.  */
SECTIONS
{
    /*
     * cs107e uses this:
     *      .text 0x8000 :  { start.o(.text*)  *(.text*) } 
     * which makes linking in start.o awkward if you don't copy it into
     * each dir.
     *
     * XXX: I cannot make text.boot work if we link start into libpi.a 
     */
    .text 0x8000 :  { 
        __code_start__ = .;
        KEEP(*(.text.boot))  
        *(.text*) 
        __code_end__ = .;
        . = ALIGN(8);
    }
    .rodata : { *(.rodata*) }
    .data : { 
            __data_start__ = .;
            *(.data*) 
            . = ALIGN(4);
            __data_end__ = .;
    } 
    .bss : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end__ = .;
        . = ALIGN(8);
        __prog_end__ = .;
        __heap_start__ = .;
    }
}
