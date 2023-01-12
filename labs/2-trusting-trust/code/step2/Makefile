# you should not need to modify this.
CC =gcc
CFLAGS = -Og -g -Wall -Wno-unused-variable

# start uncommening out these checks
all: login compiler  trojan-compiler compiler.check
 
check: login-attack.check trojan-compiler.check

login: login.c
	$(CC) $(CFLAGS) $< -o $@

compiler: compiler.c
	$(CC) $(CFLAGS) $< -o $@

trojan-compiler: trojan-compiler.c
	$(CC) $(CFLAGS) $< -o $@

# this will only work after you implement the login injection
login-attack.check: trojan-compiler
	@echo "\n----------------------------------"
	@echo "checking that the login attack works and user 'ken' has a";
	@echo "   login success without a password";
	./trojan-compiler login.c -o login-attacked
	echo "ken\n" | ./login-attacked 
	@echo "ken\n" | ./login-attacked  > out
	@diff login-attacked.out out
	@echo "success: login attack checked out!"

# this will only work after you implement self-replicating.
trojan-compiler.check: trojan-compiler
	@echo "\n----------------------------------"
	@echo "checking that the attack on cc works";
	./trojan-compiler compiler.c -o compiler-attacked
	./compiler-attacked login.c -o login-attacked > out && cat out
	@echo "compile: could have run your attack here!!" | diff out -
	@echo "success: trojan cc attack checked out!"
	@echo ""

# double check that the compiler we gave you correctly
# compiles login.
compiler.check: compiler
	@echo "------------------------------------------";
	@echo "Check that compiler compiles login";
	./compiler login.c -o login
	echo "guest\npassword\n" | ./login
	@echo "guest\npassword\n" | ./login > out
	@diff login.out out
	@echo "success: compiler compiled login correctly!"
	@echo "------------------------------------------";

clean:
	rm -f  compiler-attacked login-attacked out login compiler trojan-compiler *.o a.out  *~ compiler-attacked.c login-attacked.c out.c temp-out.c
