#define MAILBOX_FULL   (1<<31)

// status field of r/pi mailbox
static unsigned *status =  (void*)(0x2000B880+4*7);

void wait_not_full(void) {
    // wait until mailbox is not full
    while(*status & MAILBOX_FULL)
        ;
}
