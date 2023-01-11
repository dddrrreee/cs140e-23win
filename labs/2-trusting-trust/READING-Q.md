## Reading questions about the paper.

Simple example to illustrate point of paper: `while` loops do not exist
in compiler `C0`.  How do you add them?
  1. Write code to parse/implement them in copy of `C0`, call it `C1`.
  2. Compile `C1` with `C0`.
  3. Rewrite code in `C1` to use `while` loops, giving `C2`.
  4. Compile `C2` with `C1`.  Pretty easy so far, just requires some work.
  5. Can then compile `C2` with itself and *throw away C0 and C1*.
     Circular definitions can work in compilers.  This is weirder
     than the words used to describe it
  6.  Of course, you better not lose both `C1` and `C2`!

Thompson wants to add a hack to the the compiler that will:
  1. Recognize when it compiles the password program and insert a
     backdoor into it.  (Not that hard to understand.)
  2. Recognize the compiler itself and insert (1) into it.
  3. Compile (2) and throw away the source.  (As in our `while` exmaple).
  4. Thus, will automatically modify `passwd` and also will automatically
     modify a clean copy of the compiler.

Breaking down the paper:
 - program that when run will produce itself.
 - "not precisely a self-reproducing program"?   (it has things such as `\t`
   rather than `10`).
 - How does it produce a self-reproducing program?  [When you run it it 
   spits out ASCII code.]
 - "Contains an arbitrary set of baggage": where?
 - What can you delete?
 - From the code: the comment states that `s` is a representation "from `0`
   to the end". 
     - Which `0` do you mean?
     - How do you know?
     - What this point in the code?
 - Can you do this without a loop?
 - Why not `printf` the string twice?
 - Can you reverse the order?
 - "Many chicken and egg problems" --- others?  (Any contruct if you 
   compile the compiler with itself.)

Questions about his code:
  - What is interesting about 2.1 (2.2 in the text)?
    [`\n` is defined in terms of itself]
    
  - What happens if you compile 2.3 in the original compiler?
    [Parse error: `\v` does not exist.]

  - So what is the hack?  Whatever computer system your on has some
    character encoding scheme --- figure out the decimal and return that.
  
  - Why not leave it at that?  It's a magic number.  Doesn't work with other
    encodings.

  - Then compile with `\v`.
  - What's odd?  [Keep the binary: it knows `\v`.   Compile the source
    with it.  ***The compiler now defines `\v` in terms of itself***
    Very cool: circular definitions work with computer stuff.  Kind of 
    an odd induction.]

  - How does it matter that this is ascii?  [2.3: hardcode the value of 
    `\v` as 11, can then compile 2.2]

  - Where is the trojan horse?  [In the compiler]
  - How actually match?  In the parse tree or whitespace removed source?
