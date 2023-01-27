# to start with use our objects.
# for checkoff this should be 0
USE_STAFF = 1

PART ?= 1

# the different tests
# you can also just assign PROGS manually.
PROGS = $(wildcard ./1-test*.c)
PROGS = $(wildcard ./4-test*.c)
PROGS = $(wildcard ./5-test*.c)
PROGS = $(wildcard ./6-test*.c)
PROGS = $(wildcard ./7-test*.c)
PROGS = $(wildcard tests/1-test*.c)
# checkoff skips 1 since behavior changes
PROGS = $(wildcard ./[34567]-test*.c)
PROGS = $(wildcard ./1-test*.c)
PROGS = $(wildcard ./$(PART)-test*.c)

# uncomment if you want it to automatically run.
# RUN=1


# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = pi-install

##################################################################
# shouldn't have to touch below.



# use our kmalloc
DIR := $(CS140E_2023_PATH)/libpi
STAFF_OBJS := $(DIR)/staff-objs/kmalloc.o

# hack to in theory make it easy to flip b/n staff
# code and our code
ifneq ($(USE_STAFF),0)
    # to start: use our thread-asm
    STAFF_OBJS += $(DIR)/staff-objs/rpi-thread-asm.o
    # to start: use our rpi-thread.o
    STAFF_OBJS += $(DIR)/staff-objs/rpi-thread.o
    # COMMON_SRC = rtc-thread.c
else
    # as you implement the lab:
    #   - comment out each STAFF_OBJ
    #   - uncomment the corresponding COMMON_SRC
    COMMON_SRC += thread-asm.S 
    COMMON_SRC += rpi-thread.c
endif

CAN_EMIT = 0

# there must be a better way to do this.
ifeq ($(MAKECMDGOALS),checkoff)
    PROGS = $(wildcard ./[34567]-test*.c)
    USE_STAFF = 0
endif

GREP_STR := 'TRACE:\|ERROR:\|PANIC:'

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template

checkoff: check
	@echo "--------------------------------------------------------";
	@echo "Environment:";
	@echo "   STAFF_OBJS [should only have kmalloc] =<$(STAFF_OBJS)";
	@echo "   COMMON_SRC [should have both .o] =<$(COMMON_SRC)>";
	@echo -n "   total tests=";
	@ls $(PROGS:.c=.out) | wc -l
	@echo -n "   checksum of cksum: ";
	@cksum $(PROGS:.c=.out) | sort -n | cksum

.PHONEY: checkoff
