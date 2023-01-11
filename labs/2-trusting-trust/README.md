## Trusting trust

Today is a fun lab; on the teaching evals it was generally
the favorite of the quarter.  

Before lab:
  - Make sure you read the [PRELAB](./PRELAB.md)
  - Read the paper for today: [trusting-trust.pdf](./trusting-trust.pdf)
    carefully (I'd say at least three times).
  - Make sure you can answer the reading questions [READING-Q](./READING-Q.md).
  - Look through the code in `code/step1`, `code/step2` and `code/step3`:
    you will be implementing the missing pieces.
  - And of course, before lab read this README a couple times
    to see what you'll be doing.

Today we'll a simple version of the code in the short Ken Thompson paper
(`trusting-trust.pdf`).  This paper was his Turing award lecture (our
field's Nobel prize), where he discussed an evil, very slippery hack he
implemented on the early Unix systems that injected an invisible backdoor
that let him login to any system.  What is interesting about the trick:

  - You couldn't see the attack by inspecting the Unix `login` source
    because the system compiler `cc` injected the backdoor attack 
    whenever `login` was compiled.  This is devious, but conceptually
    straightforward. 

  - But you also couldn't see the attack by inspecting the `cc` compiler's
    source because the shipped compiler binary `bin/cc` would inject the
    the code that would inject the `login-backdoor` attack into `login`
    whenever `bin/cc` compiled the `cc` source code.

    This means (1) the `cc` source code was clean and contained no attack
    injection code but (2) if you compiled `cc` source with the infected
    `cc` binary, the infected `cc` would produce another the infected
    `cc` binary from the clean `cc` source.

    This hack is devious and very not conceptually straightforward.

  - The magic trick is the initial induction.  At some point, Ken wrote
    `cc` source that *did* have the attack --- if you looked at the source
    you would see it.  However, he then compiled this flawed source with
    a non-flawed `cc` compiler binary to produce an infected binary `cc`.
    He then deleted the flawed compiler source and reverted to an older
    clean source code, but kept the flawed `cc` binary.  This is all
    that was needed to do induction: when anyone recompiled the clean
    `cc` implementation using the infected `cc` it would produce another
    infected `cc` binary.

    This is a weird result that should seem to flirt with the impossible
    if you think about it long enough.

We will write the code for his hack.  While the paper is short, and the
hack seems not that hard, when you actually have to write out the code,
you'll likely realize you don't actually know the next thing to type.

Is a neat example of how circular, recursive definitions work when
you have a compiler that can compile itself.  

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
steps: start with `code/step1`, then `code/step2`, then `code/step3`.

Check-off:
   - When you type `make` in `code/step3` the test passes.

-----------------------------------------------------------------------
### step1: write a self-reproducing program.

To get started, we'll first finish implementing the self-reproducing
program (a [quine](https://en.wikipedia.org/wiki/Quine_(computing)))
based on Figure 1 in Thompson's paper.

#### What to do

Finish implementing `code/step1/quine-gen.c` which
when fed `code/step1/seed.c`  will spit out a self-contained
quine that contains (1) a character array describing the input
and (2) the input itself (as shown in the beginning of Figure 1).

  - We give you `code/step1/seed.c`: the main part of the C code in
    the paper (so you don't have to type it in).

  - When you run your finished code:

            % ./quine-gen < seed.c 

    You should get something that looks like:

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

  - You can then use `string-to-char-array` to produce a self-replicating
    quine as follows:

        # 1. generate byte array based on seed.c
        ./string-to-char-array < seed.c > quine.c
        # 2. concatenate seed.c to the emitted byte array
        cat seed.c >> quine.c
        # 3. compile the result
        gcc quine.c -o quine
        # 4. run it and put the output in kid.c
        ./quine > quine-output.c
        # 5. make sure the quine and the output it produces
        # are identical.
        diff quine.c quine-output.c
        
    If this passes: Congratulations!  This is the first step in
    replicating Thompson's hack.  If not start running each one at a
    time and look at the output.

--------------------------------------------------------------------------
#### step2: inject an attack into `step2/login` and `step2/compiler`

We now start writing a simple version of the compiler code injection
attack Thompson described.

In this second step we're going to inject trivial attacks into the 
`step2/login` and `step2/compiler` in the obvious way and check that these work.

You can do all this on your own from the Thompson paper and ignore our
code (as long as it checks out!) or do the suggestions in Step 1 and
Step 2 below.

We give you:

  - `step2/login.c`: a dumb login program.
  - `step2/compile.c`: a trivial "compiler" that just reading in a file and
    runs `gcc` on it.
  - `step2/trojan-compiler.c`: a copy of `step2/compile.c` that you will
    modify to insert "attacks".

What you will do:

You will modify a copy of `compile.c`, `trojan-compiler.c` so that:
  1. Modify `compile` in `trojan-compiler.c` so that it scans its input for
     the start of the `login` routine in `login.c` and if it finds it,
     inject a backdoor.  This is fairly easy/mechanical.

  2. Similarly: modify `compile` in `trojan-compiler.c` so that it scans its
     input for the `compile` routine and, if it finds it, injects a
     simple print statement that will print each time `compile` runs ---
     this is a placeholder for the final, subtle step.

#### Step 0: Check that `make` completes successfully.

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

#### Step 1: attack `login`

Here you'll inject an trivial attack in the login program that will make
`login` return true for user `ken` without asking for a password.

The basic approach: 
   0. To save you time we created a copy of `compiler.c` as `trojan-compiler.c`.

   1. Modify `trojan-compiler` so that if it matches the string `int login(char
      *user) {` in the input code, then inject the attack:

        if(strcmp(user, \"ken\") == 0) return 1;

      at the beginning of `login`.  Emit all other code identically.

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

#### step 2: do a dumb attack on `compiler`

Before doing the fancy Thompson trick we'll just make sure everyting
works by injecting a dumb "attack" into the compiler that will cause it
to add the `printf`:

        printf("%s:%d: could have run your attack here!!\n\", 
                                        __FUNCTION__, __LINE__);

As the very first `fprintf` in `compile`.

It should work similar to the `login` attack:

   1. Match the beginning of compile:

        static void compile(char *program, char *outname) {
            FILE *fp = fopen("./temp-out.c", "w\");
            assert(fp);"

   2. Inject the `printf` above in the emitted code.

   3. When you compile `compiler` and then compile `login` it 
      should emit:

        % ./trojan-compiler compiler.c -o cc-attacked
        % ./cc-attacked login.c -o login
        compile:19: could have run your attack here!!

Obviously, this isn't very interesting; we just do it to make debugging simple
before the next step.

#### Summary 

Cool!  These two are the easier steps just to make sure you can attack
the programs you want.  

The next one is the tricky step where you will:
    - inject the code to
    - inject these attacks 
    - into the compiler so that it can 
    - inject them into the compiler :)

You will use the trick from `code/step1` to inject a self-replicating
copy of the attack into `compiler.c` while compiling it.

Why do we have to do anything more?   The big problem of course is that 
after you replace the trojan-compiler `compiler` with the code it 
generates it will no longer contain the attack.  

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
        % ./compiler compiler.c -o compiler.2 
        compile:19: could have run your attack here!!  
        % mv ./compiler.2 ./compiler

        # and see if it still has the login attack.
        % ./compiler login.c -o login
        compile:19: could have run your attack here!!
        % ./login
        user: ken
        user <ken> does not exist
        login failed for: <ken>

        # uh oh, login attack does not work anymore.

The fancy step (next) is to fix this.


--------------------------------------------------------------------------
#### step3: inject an attack that will inject an attack into the compiler.

Finally, we use the trick from Part 1 to inject a self-replicating attack
into the compiler.

Follow the [README.md](code/step3/README.md) in `code/step3` and
combine the code from Part 1 and Part 2 so that your trojan compiler
(`trojan-compiler.c`) will take a clean, virgin copy of the "system"
compiler `compiler.c` and inject a self-replicating copy of its trojan
attack into it.

#### Postscript

You have now replicated Thompon's hack.  Startlingly, there seem to be
only a few people that have ever done so, and most that believe they
understand the paper woulnd't actually be able to write out the code.
You can probably really stand out at parties by explaining what you did.
