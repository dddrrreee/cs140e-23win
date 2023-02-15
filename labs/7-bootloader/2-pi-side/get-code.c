/*****************************************************************
 * bootloader implementation.  all the code you write will be in
 * this file.  <get-code.h> has useful helpers.
 *
 * if you need to print: use boot_putk.  only do this when you
 * are *not* expecting any data.
 */
#include "rpi.h"
#include "get-code.h"
#include "get-code.h"
//#include "simple-boot.h"
//#include "staff-crc32.h"

// wait until:
//   (1) there is data (uart_has_data() == 1): return 1.
//   (2) <timeout> usec expires, return 0.
//
// look at libpi/staff-src/timer.c
//   - call <timer_get_usec()> to get usec
//   - look at <delay_us()> : for how to correctly 
//     wait for <n> microseconds given that the hardware
//     counter can overflow.
//unsigned long code_start;
//unsigned long code_end;
//unsigned char *exec_addr;
//unsigned long put_prog_info[4];
//unsigned long get_code_info[2];
extern uint32_t  __code_start__[];
extern uint32_t  __prog_end__[];
static unsigned 
has_data_timeout(unsigned timeout) {
      unsigned start = timer_get_usec();
    while(1) {
        if(uart_has_data())
            return 1;
        unsigned curr = timer_get_usec();
        if((curr - start) >= timeout)
            return 0;
        //delay_us(10);
    }
    return 0;
    //boot_todo("has_data_timeout: implement this routine");
}

// iterate:
//   - send GET_PROG_INFO to server
//   - call <has_data_timeout(<usec_timeout>)>
//       - if =1 then return.
//       - if =0 then repeat.
//
// NOTE:
//   1. make sure you do the delay right: otherwise you'll
//      keep blasting <GET_PROG_INFO> messages to your laptop
//      which can end in tears.
//   2. rember the green light that blinks on your ttyl device?
//      Its from this loop (since the LED goes on for each 
//      received packet)
static void wait_for_data(unsigned usec_timeout) {
    while(1) {
        boot_put32(GET_PROG_INFO);
        if(has_data_timeout(usec_timeout))
            break;
        delay_us(usec_timeout);
    }
    
    //boot_todo("wait_for_data: implement this routine");
}

// IMPLEMENT this routine.
//
// Simple bootloader: put all of your code here.
int get_code(uint32_t *code_addr) {
    // 0. keep sending GET_PROG_INFO every 300ms until 
    // there is data: implement this.
    wait_for_data(300 * 1000);

    /****************************************************************
     * Add your code below: 2,3,4,5,6
     */
    uint32_t addr;
    uint32_t nbytes;
    uint32_t cksum;
    uint32_t put_prog_info;
    //uint32_t addr = 0;
    //code_start = (unsigned long)code_addr;
    //code_end = code_start + put_prog_info;
    //unsigned char exec_addr_ptr = *exec_addr;
  
        // 2. Expect: [PUT_PROG_INFO, addr, nbytes, cksum]
    put_prog_info = boot_get32();
    if (put_prog_info != PUT_PROG_INFO) {
        boot_err(BOOT_ERROR, "Error");
    }

    addr = boot_get32();
    nbytes = boot_get32();
    cksum = boot_get32();
   

// 3. Check if the binary will collide with the bootloader code
    if (((unsigned)&PUT32 > (unsigned)&code_addr) && ((unsigned)&__prog_end__ > (unsigned)&code_addr))
    //if ((addr+nbytes) >= PUT32)
    {
   // if (addr < * __prog_end__ && addr < *__code_start__) {
        //panic("Error");
        boot_err(BOOT_ERROR, "Error");
    }

    //unsigned long cksum = put_prog_info[3];

    // 4. Send [GET_CODE, cksum] back
   
    boot_put32(GET_CODE);
    boot_put32(cksum);
    //get_code_info[0] = GET_CODE;
    //get_code_info[1] = cksum;
    //boot_get32((unsigned long*)get_code_info, 2);

    // 5. Expect: [PUT_CODE, <code>]
    
    //uint32_t code;
    unsigned put_code;
    put_code = boot_get32();
    if (put_code != PUT_CODE) {
        panic("Error");
    }

    for(int i = 0; i < nbytes; i++) {
        int8_t byte = uart_get8();
        PUT8(addr + i , byte);
    }
        
    
    //code = boot_get32();
    //unsigned char buf[i];
    //code = boot_get32();

    //for (int i = 0; i < put_prog_info; i++) {
        //PUT8(code_start + i, buffer[i]);
   // }

    // 6. Verify the checksum of copied code
   // if (cksum != crc32(addr)) {
     //   panic("op code is not get code:");
    //}
    if (crc32((void*)addr, nbytes) != cksum){ 
        boot_err(BOOT_ERROR, "Error");
    }


    //return put_prog_info;


    boot_putk("UART");
    boot_put32(BOOT_SUCCESS);
    // 2. expect: [PUT_PROG_INFO, addr, nbytes, cksum] 
    //    we echo cksum back in step 4 to help debugging.
   // boot_todo("wait for laptop/server response: echo checksum back");

    // 3. If the binary will collide with us, abort with a BOOT_ERROR. 
    // 
    //    for today: assume that code must be below where the 
    //    booloader code gap starts.  make sure both the start and 
    //    end is below <get_code>'s address.
    // 
    //    more general: use address of PUT32 and __PROG_END__ to detect: 
    //    see libpi/memmap and the memmap.h header for definitions.
   // boot_todo("check that binary will not hit the bootloader code");

    // 4. send [GET_CODE, cksum] back.
    //boot_todo("send [GET_CODE, cksum] back\n");

    // 5. we expect: [PUT_CODE, <code>]
    //  read each sent byte and write it starting at 
    //  <addr> using PUT8
    //
    // common mistake: computing the offset incorrectly.
   // boot_todo("boot_get8() each code byte and use PUT8() to write it to memory");

    // 6. verify the cksum of the copied code using:
    //         boot-crc32.h:crc32.
    //    if fails, abort with a BOOT_ERROR.
   // boot_todo("verify the checksum of copied code");

    // 7. send back a BOOT_SUCCESS!
    //boot_putk("<PUT YOUR NAME HERE>: success: Received the program!");
    //boot_todo("fill in your name above");

    // woo!
    //boot_put32(BOOT_SUCCESS);

    // We used to have these delays to stop the unix side from getting 
    // confused.  I believe it's b/c the code we call re-initializes the 
    // uart.  Instead we now flush the hardware tx buffer.   If this
    // isn't working, put the delay back.  However, it makes it much faster
    // to test multiple programs without it.
    // delay_ms(500);
    uart_flush_tx();

    *code_addr = addr;
    return 1;
}
