BOOTLOADER=my-install

START = staff-start.o


PROGS  += 1-srs-rfe.c
PROGS  += 2-simple-single-step.c
PROGS  += 3-reg-save.c
PROGS += 4-reg-restore.c
PROGS += 5-single-step-one.c
PROGS  = 0-user-sp-lr.c

# part 6: final
# PROGS = 6-single-step-proc.c

# have a common library for the tests
LIB = libasm.a

# part 0
STAFF_LIB_OBJS += staff-0-user-sp-lr-asm.o
# LIB_SRC += 0-user-sp-lr-asm.S  

# part 1
# these are the files used in the library.
# LIB_SRC += 1-srs-rfe-asm.S  
STAFF_LIB_OBJS += staff-1-srs-rfe-asm.o 

# part 2
#LIB_SRC += 2-simple-single-step-asm.S breakpoint.c
STAFF_LIB_OBJS += 2-simple-single-step-asm.o staff-breakpoint.o

# part 3
# LIB_SRC += 3-reg-save-asm.S
STAFF_LIB_OBJS += staff-3-reg-save-asm.o

# part 4
# LIB_SRC += 4-reg-restore-asm.S
STAFF_LIB_OBJS += staff-4-reg-restore-asm.o

# LIB_SRC += 5-single-step-one-asm.S
STAFF_LIB_OBJS += staff-5-single-step-one-asm.o

# automatically run.
RUN=1

CAN_EMIT=1

EXCLUDE ?= grep -v simple_boot
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2
