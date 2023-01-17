NAME=main

# we now use the -MMD flag
CFLAGS = -O -Wall -MMD

# now we use .d files for dependencies
src = $(wildcard *.c)
obj = $(src:.c=.o)
dep = $(src:.c=.d)

all: $(NAME) test

$(NAME): $(obj)
	$(CC) $(CFLAGS) $(obj) -o $(NAME)

# include the generated dependency rules.
# open up a .d file in your editor to see 
# why this works!
-include $(dep)

%.o: %.c 
	$(CC) $(CFLAGS) -c $<  -o $@

.PHONY: clean
.PHONY: test

test: $(NAME) 
	./$(NAME) > out
	diff out $(NAME).ref
	@echo "makefile: program '$(NAME)' passed test"
	rm -f out

clean:
	rm -f $(NAME) out *~ *.bak *.o  *.d
