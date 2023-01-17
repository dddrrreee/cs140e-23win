# have a single variable to define the program
# gather up all source and build and assume all
# source depends on all headers. 
NAME=main
CFLAGS = -O -Wall

# get all headers: will make everything depend on them.
hdr = $(wildcard *.h)
src = $(wildcard *.c)
# get all source
obj = $(src:.c=.o)

all: $(NAME) test

$(NAME): $(obj) $(hdr)
	$(CC) $(CFLAGS) $(obj) -o $(NAME)

%.o: %.c $(hdr)
	$(CC) $(CFLAGS) -c $<  -o $@

.PHONY: clean
.PHONY: test

test: $(NAME) 
	./$(NAME) > out
	diff out $(NAME).ref
	@echo "makefile: program '$(NAME)' passed test"
	rm -f out

clean:
	rm -f $(NAME) out *~ *.bak *.o 
