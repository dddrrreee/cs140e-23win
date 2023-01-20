NAME=main

# we use the source code and headers from the previous 
# examples directory
src = $(wildcard ../4-example/*.c subdir/*.c)

# all the directories  we use
dirs := $(sort $(dir $(src)))

# relocate names to the build_dir to make later substitutions
# easy
reloc := $(foreach o, $(src), $(notdir $o))

# compute .o and .d names
obj := $(reloc:.c=.o)
dep := $(reloc:.c=.d)

# make flags that tell compiler to look in each of 
# these dirs for headers
inc := $(foreach d, $(dirs), -I$d)
CFLAGS = -O -Wall -MMD $(inc)

# the environment that the compilation takes place in.
# should rerun if (1) change the makefile and (2) must
# make the build_dir before anything.
env = $(firstword $(MAKEFILE_LIST))

# tell make where to search for prereqs.
VPATH := $(dirs)

all:  $(NAME) test

# print out the variables we have computed.
print:
	echo "src=<$(src)>"
	echo "obj=<$(obj)>"
	echo "dep=<$(dep)>"
	echo "dirs=<$(dirs)>"
	echo "inc=<$(inc)>"
	echo "mk=<$(mk)>"

$(NAME): $(obj) $(env)
	$(CC) $(CFLAGS) $(obj) -o $(NAME)

-include $(dep)

%.o: %.c $(env)
	# $$< = <$<>   $$@ = <$@>
	$(CC) $(CFLAGS) -c $<  -o $@

.PHONY: clean
.PHONY: test

test: $(NAME)
	./$(NAME) > out
	diff out $(NAME).ref
	@echo "makefile: program '$(NAME)' passed test"
	rm -f out


clean:
	rm -rf $(NAME) out *~ *.bak *.d *.o

.PHONY: clean test print
