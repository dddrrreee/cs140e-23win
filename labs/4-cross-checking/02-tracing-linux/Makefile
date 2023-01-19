# trivial makefile for compiling NAME  and wrapping up foo.
#  - useful improvement: given a list of routines, generate all
#    the wrap arguments to ld.
NAME = trace-foo
SRC = trace-foo.o foo.o
HDRS =  foo.h

# tell ld to wrap foo
# this is passed to ld
WRAP_FLAGS = -Wl,--wrap=foo

CFLAGS = -O -Wall -Wno-unused-function -Wno-unused-variable $(WRAP_FLAGS)

all: $(NAME)  run

$(NAME): $(SRC) $(HDRS)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME) 

run: 
	@echo "if --wrap worked: should see calls to both 'wrap_foo' and 'real_foo'";
	./$(NAME)
	

clean:
	rm -rf $(NAME) *~ *.o Makefile.bak  

.PHONY: all clean  run
