# to use your gpio-int.c swap these.
# COMMON_SRC := ../2-gpio-int/gpio-int.c
STAFF_OBJS := ../staff-objs/gpio-int.o

# comment out if you want to use your libpi
LPI = ../staff-objs/libpi.a

# level 3: do N iterations of event type.
# easiest: one type of event.
PROGS = 3-N-rising-edge.c
# harder: rise + fall
PROGS = 3-N-rise-fall.c
# hardest: everything.
PROGS = 3-N-rise-fall-timer.c

# level 2: combine more than one event type
# easier
PROGS = 2-rise-fall-edge.c
# has timers so is harder.
PROGS = 2-timer-rise-fall-edge.c

# level 1: easiest: do a single event once 
# (other than for timer)
PROGS = 1-falling-edge.c
PROGS = 1-rising-edge.c
PROGS = 1-timer.c

# everything.
PROGS = $(wildcard [123]-*.c)

COMMON_SRC += test-interrupts.c
COMMON_SRC += interrupt-vec.c interrupt-asm.S 

# uncomment if you want it to automatically run.
# RUN=1

CAN_EMIT=0
EXCLUDE ?= grep -v simple_boot
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

# do a checkoff.
checkoff:
	make -f checkoff.mk
