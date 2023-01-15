## Trusting trust

Today is a fun lab; on the teaching evals it was generally
the favorite of the quarter.  

Before lab:
  - Make sure you read the [PRELAB](./PRELAB.md)
  - Read the paper for today: [trusting-trust.pdf](./trusting-trust.pdf)
    carefully (I'd say at least three times).  To check your understanding
  - Make sure you can answer the reading questions [READING-Q](./READING-Q.md).
  - Look through the code in `code/step1`, `code/step2` and `code/step3`:
    you will be implementing the missing pieces.
  - And of course, before lab read this README a couple times
    to see what you'll be doing.

Today we'll a simple version of the code in the short Ken Thompson paper
(`trusting-trust.pdf`).  This paper was his Turing award lecture (our
field's Nobel prize), where he discussed an evil, very slippery hack he
implemented on the early Unix systems that injected an invisible backdoor
that let him login to any Unix system.  

We will write the code for his hack.  While the paper is short, and the
hack seems not that hard, when you actually have to write out the code,
you'll likely realize you don't actually know the next thing to type.

The hack is a neat example of how circular, recursive definitions
work when you have a compiler that can compile itself.    For example,
a C compiler written in C implements `while` loops,
`for` loops and `if` statements *using code written with `while` loops,
`for` loops and `if` statements*.  Seems impossible.  Didn't Godel say
something about it?    After the lab you'll have a bit firmer grip
on this slippery weirdness.   (If you like this kind of magic trick,
read about about "self-hosting" or 
[bootstrapping](https://en.wikipedia.org/wiki/Bootstrapping_(compilers))
compilers.)


Everyone I've met above a certain age in systems has read this paper
and they all seem to think they understand it.  However, I've not met
a single one that had ever written the code to do it.  There is a big
difference between passively nodding to nouns and verbs and being able
to actively construct an implementation of an idea.

This is a pretty commmon pattern both in systems in general and this class
in particular: you (especially I) can read a bunch of prose, think you
understand, and then when you fire up the editor to type out the first
line of code, realize you actually don't know what to type at all,
and the apparent understanding was fake.  Fortunately, the brute force
solution is easy: just keep writing the code, and when you're done,
you'll understand much more deeply in a way that cannot be faked.
(Hence this class.)

For this lab you will write code to implement Thompson's hack in three
steps: start with `code/step1` (easy), then `code/step2` (medium), 
then `code/step3` (hardest).

Standard check-off:
  - When you type `make check` in `code/step3` the test passes.

    NOTE: the most common mistake on this lab: making code much more
    complicated by implementing the attacks by buffering and manipulating
    temporary `char` arrays rather than emiting the code immediately to
    the file using `fprintf`.  Quick test: your code shouldn't be using
    `strcpy` or `strcat` --- not needed and creates bugs.

Hard check-off (if you're exceptionally ambitious):

  - Do not use our code at all but write everything on your own
    based entirely on the paper.  This will maximize difficulty, but also
    understanding. Just make sure you name your programs
    so that they pass our `make check` in `step3`.

    NOTE: in general for any lab you can always ignore our code and just
    implement your own from scratch as long as the provided tests pass.
    This will count as an extension (either major or minor depending on
    the lab).

Extensions:
  - Redo everything in a different language (e.g., rust).
  - Inject attacks into a binary program rather than source code.
  - Attack a different program (this could be a minor or major extension
    depending.)

-------------------------------------------------------------------
### Intuition: self-replicating attack injection

What is interesting about his hack:

  - You couldn't see the attack by inspecting the Unix `login` source
    because the system compiler `cc` injected the backdoor attack 
    whenever `login` was compiled.  

    This is devious, but conceptually straightforward, what makes the
    attack subtle is that it was self-replicating:

  - You also couldn't see the attack by inspecting the `cc` compiler's
    source because the attack was not in there either.  

    The attack lived only in the shipped compiler binary, which detected
    when it was compiling the clean, non-hacked compiler source code
    and automatically injected a self-replicating copy of the attack
    code into the produced binary.  This flawed binary was then able to
    inject attacks both into login and into the original compiler source.

    Devious and not straightfordward.

Words make this awkward.  To make it concrete, assume
we have three single-file programs:

  - `compiler.c`: the clean unhacked compiler, with no attack.
    It can compile itself:
    
            # generate a new compiler binary
            % compiler compiler.c -o compiler

  - `login.c`: the clean unhacked login program.   When run
    it prompts for a user name.  If the user does not exist
    it whines and exits.  For example:

            % compiler login.c -o login
            % login
            username: ken
            Not such user: exiting.

  - `trojan-compiler.c`: a hacked version of `compiler.c` that 
    can inject an attack into login:

            # inject a backdoor when compiling login.c
            % trojan-compiler login.c -o login

            # login now has the backdoor and lets ken in.
            % login
            username: ken
            Successful login!

    Because the compiler automatically injects the backdoor into `login.c`
    during compilation, it is impossible to inspect `login.c` to see
    the attack.  (Note: we ignore the fact that `login` would be 
    for remote access so that the transcripts are easier.)

    This is cool, but not that tricky to follow.  The big leap that
    requires some thought is that `trojan-compiler.c` contains the code
    to *also* automatically injects its entire set of attacks into a clean
    `compiler.c` during compilation.  (I.e., self-replicate its attack.)

    When this means is that after you compile `compiler.c` with 
    `trojan-compiler` you get the following magic trick:

            % trojan-compiler compiler.c -o compiler
            % compiler login.c -o login
            % login
            username: ken
            Successful login!

    In other words, during compilation the `trojan-compiler` binary
    essentially turns `compiler.c` into `trojan-compiler.c` and generates
    a binary from it. This new hacked `compile` binary will now inject attacks
    both into `login.c` and `compiler.c` just as `trojan-compiler` does.
    Further, the hacked `compiler` binary also self-replicates the
    attack when used to compile itself *even though the attack is not in
    `compiler.c`*!  (Think about this weirdness: it's as close to a koan
    as I know in systems.)

    To take this a step further, we can even delete everything to do
    with the hack and recompile `compiler.c` with its flawed binary over
    and over and have all the evil self-replicate:

            % trojan-compiler compiler.c -o compiler
            % rm trojan-compiler trojan-compiler.c
            % compiler compiler.c -o compiler
            % compiler compiler.c -o compiler
            % compiler compiler.c -o compiler
            % compiler compiler.c -o compiler
            # ... doesn't matter how many times ...
            % compiler compiler.c -o compiler
            % compiler compiler.c -o compiler
            % compiler compiler.c -o compiler
            % compiler login.c -o login
            % login
            username: ken
            Successful login!

    I.e., once the attack is injected into the `compiler` binary, it
    is equivalant to the original `trojan-compiler` binary, even though
    there is no `trojan-compiler` or `trojan-compiler.c` on the system
    and we have compiled `compiler.c` with `compiler` many times.

    And to repeat: if you look in `compiler.c` there is no attack.
    If you look in `login.c` there is no attack.    

    This is a weird result that should seem to flirt with the impossible.

-----------------------------------------------------------------------
### step1: write a self-reproducing program generator

To get started, we'll first finish implementing the self-reproducing
program (a [quine](https://en.wikipedia.org/wiki/Quine_(computing)))
based on Figure 1 in Thompson's paper.

You are given:

  - `code/step1/seed.c`: the main part of the C code in the paper (so
     you don't have to type it in).

   - the `check` recipe in the `Makefile` for making and checking
     the program (see below).  If `make check` passes in `code/step1`
     then your code works.


#### What to do

Finish implementing `code/step1/quine-gen.c` which when fed
`code/step1/seed.c`  will spit out a self-contained quine that contains
(1) a character array describing the input `seed.c` and (2) the input
itself.  This will be the code as shown at the beginning of Figure 1.

  - After you implement `quine-gen.c` and then run it:

            % ./quine-gen < seed.c 

    You should get something that looks something like:

            char prog[] = {
	            47, 	47, 	32, 	99, 	97, 	110, 	
                32, 	112,    117, 	116, 	32, 	97, 	
                ...
	            125, 	10, 0 };
            // can put any payload (held in a character array as in Thompson's
            // paper) here, before the rest of the code

            // Don't modify below.
            #include <stdio.h>

            // the C code for thompson's replicating program, more-or-less.
            int main() { 
            ...


    This is all you need to automatically generate the self-reproducing
    program based on `Figure 1`.  This will hopefully shake out any
    ambiguity in the paper.

  - You can then test that your `quine-gen` will produce a quine program
    that will emit itself as follows:

	    # 1. Generate the paper's quine from seed.c
	    % ./quine-gen < seed.c > quine.c
	    # 2. Use quine.c to generate itself
	    % cc quine.c -o quine
	    % ./quine > quine-out.c
	    # 3. Check generated quine-out.c matches quine.c
	    % diff quine.c quine-out.c
        
    If the `diff` matches: Congratulations!  This is the first step in
    replicating Thompson's hack.  If not start running each one at a
    time and look at the output.
  
    As mentioned above, to make this easier, we've added a target `check`
    in the `step1/Makefile`: you can check using `make check`.


Notes:
  1. We probably should have picked a better name for `quine-gen` --- it
     doesn't really generate a quine from arbitary input but instead merely
     converts its input to an array and then emits the array and the input.
     This naming caused some confusion when people did step 3.

  2. If you get syntax errors in the quine file you generate it --- just
     open up the file in your editor and see what you would need to fix
     these errors and then put those fixes in `quine-gen.c`.  Just because
     you automatically emitted the code rather than typed it, the rules
     are the same: treat the code as if you wrote it.

--------------------------------------------------------------------------
#### step2: inject attacks into `step2/login` and `step2/compiler`

In this second step we're going to inject trivial attacks into the
provided `step2/login.c` and `step2/compiler.c` programs in the obvious
way and check that these work.  These will not be self-replicating:
we are just trying to get what the attack code looks like --- step 3
makes these self-replicate.

Again: As noted at the start of the lab: You can do all this on your own
from the Thompson paper and ignore our code (as long as it checks out!).

We give you:

  - `login.c`: a trivial, clean login program that has no attack.
  - `compile.c`: a trivial clean "compiler" that just reads in a file and
    runs `gcc` on it.  It has no attack.
  - `trojan-compiler.c`: a copy of `compile.c` that you will modify to
    insert attacks.

What you should do:

  0. Before modifying anything make sure the code works.
     When you run `make` you should see something like:

        % make
        gcc -Og -g -Wall login.c -o login
        gcc -Og -g -Wall compiler.c -o compiler
        ------------------------------------------
        going to check that compiler compiles login
        ./compiler login.c -o login
        echo "guest\npassword\n" | ./login
        user: passwd: successful login: <guest>
        diff login.out out
        success: compiler compiled login correctly!

  1. Modify the `compile()` routine in `trojan-compiler.c` so that it
     scans its input looking for the start of the `login()` routine in
     `login.c`.  If it finds a match, it should inject a backdoor that
     lets user `ken` in without a password.  This should be fairly
     easy/mechanical.  From the intuition section above, it should
     work as follows:

            % compiler login.c -o login
            % login
            username: ken
            Invalid user!

            % trojan-compiler login.c -o login
            % login
            username: ken
            successful login: <ken>

  2. Finally, also modify the `compile()` routine in
    `trojan-compiler.c` so that it scans its input for the `compile()`
    routine in `compiler.c` and, if it finds it, injects a simple print
    statement that will print each time `compile()` runs --- this is
    obviously not a useful "attack", but rather rather serves as a
    placeholder for the final, subtle step in part 3.

##### More details about attacking `login`

As stated, you'll first inject an trivial attack in the `login` program
that will make the `login()` routine return true for user `ken` without
asking for a password.

The basic approach: 

   1. Modify `trojan-compiler.c` so that if it matches the string
      `int login(char *user) {` in the input code, then inject the attack:

            if(strcmp(user, \"ken\") == 0) return 1;

      at the beginning of `login()`.  Emit all other code identically.

   2. When you type `ken` as a user, `login` should give you success:

            % ./trojan-compiler login.c -o login-attacked
            % ./login-attacked 
            user: ken
            successful login: <ken>

      And, of course, the one other user should still work:

            % ./login-attacked
            user: guest
            passwd: password
            successful login: <guest>

##### Details of attacking `compiler`

Before doing the fancy Thompson trick we'll just make sure everyting
works by injecting a dumb "attack" into the `complile()` routine
in `compiler.c` that will cause it to add the `printf`:

        printf("%s:%d: could have run your attack here!!\n\", 
                                        __FUNCTION__, __LINE__);

As the very first `printf` in `compile()`.

It should work similar to the `login` attack:

   1. Match the beginning of the `compile()` routine:

        static void compile(char *program, char *outname) {
            FILE *fp = fopen("./temp-out.c", "w\");
            assert(fp);"

   2. Inject the `printf` above in the emitted code.

   3. When you compile `compiler` and then compile `login` it 
      should emit:

        % ./trojan-compiler compiler.c -o cc-attacked
        % ./cc-attacked login.c -o login
        compile:19: could have run your attack here!!

Obviously, this isn't very interesting; we just do it to make debugging
simple before the next step.

Cool!  These two are the easier steps just to make sure you can attack
the programs you want.  

--------------------------------------------------------------------------
#### step3: inject an attack that will inject an attack into the compiler.

Clearly we have to develop the compiler injection "attack" since it only
prints an annoying message rather than doing something evil.  But what
exactly does it have to do?   Our problem is that we need the attack to
be self-replicating.

So, for Ken, if anyone ever re-compiled the system C compiler and
replaced his binary of it that contains his attack, the attack is gone.
For example using our toys to see the tragedy:

        # compile trojan
        % ./compiler trojan-compiler.c -o trojan-compiler  

        # replace "system" compiler
        % mv ./trojan-compiler ./compiler

        # make sure we can attack with the trojan system compiler
        % ./compiler -o login.c -o login
        % ./login
        user: ken
        successful login: <ken>   # great, still works!

        # now recompile the system compiler using the 
        # attacked compiler
        % ./compiler compiler.c -o compiler
        compile:19: could have run your attack here!!  

        # and see if it still has the login attack.
        % compiler login.c -o login
        compile:19: could have run your attack here!!
        % ./login
        user: ken
        user <ken> does not exist
        login failed for: <ken>

        # uh oh, login attack does not work anymore.

The fancy step (next) is to use the trick from `code/step1` to fix
this problem by injecting a self-replicating copy of the attack into
`compiler.c` while compiling it.  

This may or may not help, but:

  - In a sense you can see step 2 as implementing an `attack.c`
    that is roughly the source code difference between `compiler.c`
    and `trojan-compiler.c`, and this step (3) turns the attack into
    `attack-quine.c`

I'll give some hints below, but you're more than welcome to do this
on your own!  Just make sure you that you make a copy of your trojan
(`trojan-compile2`) that injects a self-replicating attack and make sure
the binary it generates when compiling a clean, virgin copy of the system
compiler is the same:

    # checking that the attack will generate itself
   
    # 1. compile compile.c and generate an attacked binary.
    % ./trojan-compile2 ../step2/compiler.c -o attacked-compiler.0

    # 2. compile compiler.c with the attacked copy
    % ./attacked-compiler.0 ../step2/compiler.c -o attacked-compiler.1

    # 3. make sure they are the same!
    % diff attacked-compiler.0 attacked-compiler.1

    # yea!  at this point we will automatically regenerate our attack
    # whenever someone compiles the system compiler.

    # 4. NOTE: step 3 is way too strong since it assumes same input 
    # to gcc gives the same output (e.g., no embedded time stamps etc).  
    # If it succeeds we know we have the same, but if it fails it doesn't 
    # mean we have a problem --- the real test is the login.
    % ./attacked-compiler.1 ../step2/login.c -o login-attacked
    % ./login-attacked
    user: ken
    successful login: <ken>
    
    # success!


### Hints

The basic idea is to take your attack and create a self-replicating version
using the code in `step1`:
  1. You'll have to generate an array of ASCII values of your attack code
     as in `step1`.
  2. You'll have to modify your attack on the compiler to inject both this
     array and a printed version of it (i.e., the code) into the compiler
     you are attacking.  This is why we looked at self-replicated programs.

Overall this doesn't take much code.  You don't have to do things this
way but: In order to make it easy to regenerate the attack as I changed
it, I used an include to pull in the generated sort-of quine code:

  1. Seperate out your attack into its own file (e.g., `attack.c`).
  2. Use `step1/gen-quine` to produce a file `attack.c` that has
     the array and the source code for the attack.
  3. Include the file into `trojan-compile2.c`: note, this `#include`
     will be in the middle of your `compile` routine, not at the 
     top of the file (where it wouldn't do anything).
  4. Profit.

Note:
  - One thing you should *not* do is read in some code at runtime (other
    than the input) and use that as part of the attack.  We want to 
    be able to copy the infected `compile` to another machine and 
    have it able to start replicating the attack rather than immediately
    break because it depended on some file on the original machine.

    If the `diff` check in the makefile fails, it could be because you
    did this mistake.  (It could also just be failing because there is
    non-determinism in compilation on MacOS M1's laptops.)
	
-----------------------------------------------------------------------
#### Postscript

You have now replicated Thompon's hack.  Startlingly, there seem to be
only a few people that have ever done so, and most that believe they
understand the paper woulnd't actually be able to write out the code.
You can probably really stand out at parties by explaining what you did.

***Lab food paid for by: Michelle J!***
