typedef struct {
  unsigned read;
  unsigned padding[3];
  unsigned peek;
  unsigned sender;
  unsigned status;
  unsigned configuration;
  unsigned write;
} mailbox_t;

enum { MAILBOX_FULL =  (1<<31) };

// mailbox device address
mailbox_t *mbox =  (void*)0x2000B880;

// wait until mailbox status indicates space
void mb_wait_notfull(void) {
    while(mbox->status & MAILBOX_FULL)
        ;
}

void notmain(void){}
