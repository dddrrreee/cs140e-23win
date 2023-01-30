#ifndef __SIMPLE_BOOT_H__
#define __SIMPLE_BOOT_H__

// protocol values shared between the pi and unix side.
//
// these could all by 1 byte, which would reduce the number
// of bytes sent.  we do 32-bit instead so we can detect 
// corruption more reliably.
enum {
    ARMBASE=0x8000, // where program gets linked.  we could send this.

    // the weird numbers are to try to help with debugging
    // when you drop a byte, flip them, corrupt one, etc.
    BOOT_START      = 0xFFFF0000,

    GET_PROG_INFO   = 0x11112222,       // pi sends
    PUT_PROG_INFO   = 0x33334444,       // unix sends

    GET_CODE        = 0x55556666,       // pi sends
    PUT_CODE        = 0x77778888,       // unix sends

    BOOT_SUCCESS    = 0x9999AAAA,       // pi sends on success
    BOOT_ERROR      = 0xBBBBCCCC,       // pi sends on failure.

    PRINT_STRING    = 0xDDDDEEEE,       // pi sends to print a string.

#if 0
    // if you want to be fancy, you could uncomment this and return
    // more precise errors.

    // error codes from the pi to unix
    BAD_CODE_ADDR   = 0xdeadbeef,
    BAD_CODE_CKSUM  = 0xfeedface,
#endif
};

#endif
