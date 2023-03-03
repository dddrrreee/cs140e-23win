PROGS := $(wildcard tests/3-*.c)
PROGS := $(wildcard tests/2-*.c)
# 1. send one way no ack
PROGS := $(wildcard tests/1-*.c)
# 0. print configs
PROGS := $(wildcard tests/0-*.c)

# all the tests
PROGS := $(wildcard tests/[0123]-*.c)

LIBS += ./libgcc.a

COMMON_SRC := nrf-hw-support.c nrf-public.c nrf-driver.c

STAFF_OBJS := $(CS140E_2023_PATH)/libpi/staff-objs/new-spi.o
STAFF_OBJS += staff-nrf-driver.o


# uncomment if you want it to automatically run.
RUN=1

# define this if you need to give the device for your pi
BOOTLOADER = my-install

# GREP_STR := 'HASH:\|ERROR:\|PANIC:\|SUCCESS:\|NRF:'
EXCLUDE ?= grep -v simple_boot
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2
