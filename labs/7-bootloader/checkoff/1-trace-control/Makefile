# start with this one if you have trouble.
TESTS = hello.bin
TESTS = $(wildcard ./*.bin)

# should not need this!
# TTYUSB = 

BOOTLOADER = my-install --trace-control

# we only look at this output
GREP_STR := "TRACE:"

include $(CS140E_2023_PATH)/libpi/mk/Makefile.check-v2
