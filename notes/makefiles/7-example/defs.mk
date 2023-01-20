# the different tools we use.
ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
AS  = $(ARM)-as

# The gcc compiler flags
#    -Wall: warn about a bunch of suss things.
#    -Werror = must fix errors
#    -O: optimize (which allows for more -Wall error checking)
CFLAGS ?= -O -Wall -Wno-unused-variable -Werror \
          -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  

# the assembler flags (for start.S)
ASFLAGS ?= --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk
