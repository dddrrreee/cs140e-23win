CC =gcc
CFLAGS = -Og -g -Wall

# implement trojan-compiler2 and then trojan-compiler2.check should work.
all: auxilary trojan-compiler2 # check 

# gross: so we can use the stuff from these steps.
auxilary:
	make -C ../step1

attack-quine.c: auxilary attack-seed.c
	../step1/quine-gen  < attack-seed.c > attack-quine.c

# you probably have to generate some stuff for this to work.
trojan-compiler2: trojan-compiler2.c attack-seed.c attack-quine.c
	$(CC) $(CFLAGS) $< -o $@

# we just duplicate stuff from step2 rather than get fancy.  kinda 
# gross, but is simple.
check:  trojan-compiler2 
	@echo "\n----------------------------------";
	@echo "checking that the attack will generate itself";
	./trojan-compiler2 ../step2/compiler.c -o cc-attacked
	./cc-attacked ../step2/compiler.c -o cc-attacked2

    # this test is way too strong since it assumes same input to gcc gives
    # the same output (e.g., no embedded time stamps etc).  if it succeeds
    # we know we have the same, but if it fails it doesn't mean we have
    # a problem --- the real test is the login.

	diff cc-attacked cc-attacked2
	@echo "success: trojan generated itself!"
	@echo ""

	@echo "\n----------------------------------"
	@echo "checking that the login attack will work."

	@echo "\n----------------------------------"
	@echo "checking that the login attack works and user 'ken' has a";
	@echo "   login success without a password";
	./cc-attacked2 ../step2/login.c -o login-attacked
	echo "ken\n" | ./login-attacked 
	@echo "ken\n" | ./login-attacked  > out
	@diff ../step2/login-attacked.out out
	@echo "success: login attack checked out!"

clean:
	make -C ../step1 clean
	make -C ../step2 clean
	rm -f  trojan-compiler2 *.o *~  cc-attacked cc-attacked2 temp-out.c out login-attacked.c login-attacked attack-quine.c

.PHONEY: auxilary check clean
