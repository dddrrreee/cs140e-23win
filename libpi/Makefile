# simple definitions for making libpi.a
#  - you should only ever have to modify SRC or STAFF_OBJS

# copy any file you want to add to libpi.a to the libpi/src
# directory.  and then add it to this variable.  
#
# for example if you want to add your gpio.c, 
#   1. copy gpio.c to src/gpio.c
#   2. add it to the SRC variable:
#           SRC += ./src/gpio.c
#   3. remove it from STAFF_OBJS
#
# tip: if you want to automatically add all .S and .c files 
# in src/ do:
#   SRC := $(wildcard ./src/*.[Sc])
#
# currently not defined to hold anything
SRC :=

# if you want to use our gpio.o or other .o (e.g., if yours
# is acting weird):
#   1. remove the file from SRC and 
#   2. add it to STAFF_OBJS 
#
# currently: use our uart.o (for printk) and gpio.o
STAFF_OBJS  +=  ./staff-objs/gpio.o
STAFF_OBJS  +=  ./staff-objs/uart.o
STAFF_OBJS  +=  ./staff-objs/gpio-pud.o
STAFF_OBJS  +=  ./staff-objs/gpio-int.o 

# automatically add all staff src from:  libc, 
# staff-src and ./staff-start.S
#
# you should not have to modify this variable.  
SRC :=  $(SRC)                              \
        $(wildcard ./libc/*.[Sc])           \
        $(wildcard ./staff-src/*.[Sc])      \
        staff-start.S

DEPS += ./Makefile ./manifest.mk

# hack to minimize git conflicts: we do various customizations
# in there; but probably would be clearer to inline it.
include ./manifest.mk
