# How to debug effectively

A lot of students come to us and say something along the lines of "my code
doesn't work, and I don't know why." However, giving us a wall of code which
doesn't work isn't particularly conducive to getting help; there are a million
things which could be the issue, and "doesn't work" is incredibly vague. We're
not psychic, and we don't have any more insight into the code than you do;
we're probably just going to tell you to do what this guide describes, since
it's really the only way to figure out what's going on.

You might have learned in school about the scientific method. This is pretty
much the approach you want to use to debug, so this guide is written in terms
of it. As a reminder, the scientific method breaks problem-solving into
a series of steps along the lines of:

1. Make an **observation**
2. Ask a **question**
3. Form a **hypothesis** (a testable explanation)
4. Make a **prediction** based on the hypothesis
5. Run an **experiment** to test the prediction
6. **Iterate** by using the results of your experiment to make new hypotheses
   or predictions

These steps, as they relate to debugging, are broken down more below.

In general, try to do at least steps 1 and 2 _before_ coming to us for help;
they provide the bare minimum of information we need to be able to say anything
useful. Ideally also run through steps 3-6 a few times on your own to rule out
any hypotheses you can think of.

## Observation

In general, when you think you've run into a bug, you want to determine two
things:

1. What is the nature of the bug? For example, does your program
   crash/segfault? Does it "hang" (loop forever)? Does an assertion fail? Does
   it do the wrong thing? Is the output corrupted? These are all very
   different kinds of bugs, and they're resolved in very different ways.

   You should be able to boil down your description of the bug to a sentence
   along the lines of "I expect my code to do X, but instead it does Y".

2. Where is the bug? Specifically, what is the first point where the actual
   behavior of your code differs from the expected behavior of your code. This
   might seem obvious and/or impossible, but there are some very simple
   strategies to at least _estimating_ a location for the bug.

   The most simple way, which is what we ask students to do in 90% of cases
   when they need help debugging, is to use binary search. You have some upper
   and lower bound on where the bug is (it's between the start and end of your
   program, if nothing else). Add a print statement somewhere in the middle,
   and see if your bug is before or after it. Now you have a narrower bound;
   split it in half again and add a print statement. Repeat until you've
   narrowed it down as far as you can.

When you're doing this binary searching, be smart about where you're splitting
your code. Bugs tend to pop up in some places more than others:

- entering or exiting loops
- conditionals
- entering or exiting functions
- pointer arithmetic
- anything involving strings or buffers

Place your print statements before and after (and within) these to rule
things out quickly.

There's one more non-technical observation you should probably also make, which
is: Has anyone else already had the same bug? If so, talk to them and they can
probably help you (or you can work together and debug twice as fast).

## Question

Now you need to make a question that you want to test. The exact question you
come up with should be specific to the problem you observed.

For example:

- "Why doesn't my code exit this loop?"
- "Why is my call to `strcat` causing a segfault?"
- "Why is my code printing out garbage?"
- "Why is the wrong branch of this `if` statement being executed?"
- "Why does my code never return from this function?"

You definitely do not want to be vague here. "Why is my code not working?" is
not a productive question, nor is "Why is the computer not doing what I told it
to?".

## Hypothesis

Now that you have a question, make a guess about the answer.

- If your code doesn't exit a loop, your loop condition might be wrong.
- If your call to a string-manipulation function segfaults, you might be
  writing past the end of an array.
- If your code prints out garbage, you might have forgotten to null-terminate
  your string.
- If your code takes the wrong branch of a conditional, you might have flipped
  your condition.
- If your code never returns from a function, you might have corrupted the
  return address on your stack.

For any given problem, there are a bunch of valid hypotheses; the ones given
above are just examples. Make sure your hypothesis is relevant to the problem
and _testable_; i.e., you can think of a modification you can make to your code
that would conclusively determine whether or not it's true. If you're at a loss
at this stage, your question might be too vague.

This is the stage where we can really help you; if you give us a specific
question you've come up with, we can come up with hypotheses for you to test.

## Prediction

You have a guess as to why your code doesn't work as intended. Now make
a prediction based on your hypothesis. This should be something concrete that
you can easily test, which should be true if your hypothesis is true and false
if your hypothesis is false.

For example:

- If a bug is caused by a loop condition being wrong, you should see the wrong
  boolean when you print out the loop condition.
- If a bug is caused by writing past the end of an array, making the array
  bigger should resolve it.
- If a bug is caused by forgetting to null-terminate your string, inserting
  a null terminator should fix it.
- If a bug is caused by a conditional being wrong, negating the condition
  should fix it.
- If your bug is caused by stack corruption, moving variables to the heap
  should fix it.

Again, there are multiple different predictions you can make for each
hypothesis. They're all valid options, and it may be worth testing multiple
just to make sure your hypothesis is right. The ones given here are just
examples. Your prediction could be as simple as "if I print out X, its value
will/won't be Y"!

## Experiment

Design an experiment to test your prediction. Remember that you can ask your
computer questions via code, and it will answer truthfully. Don't just sit
there staring at your code trying to spot the bug, be proactive and gather as
much information as you can.

Some common ways to run debugging experiments:

- Print something out.

  If you're testing control flow, this can be as simple as "took true branch of
  if statement". If you're testing data flow, you should print out all relevant
  data to try and figure out where your invalid data is coming from.

- Turn on an LED.

  Sometimes you can't add a print statement for whatever reason (they're slow,
  they're big and clunky, they mess up your code). Since we're on baremetal,
  we can easily turn on or off an LED instead. Yes, this is only a few bits of
  information (one per LED you have), but sometimes that's all you need.

- Exit early or loop infinitely.

  Sometimes you can't even turn on an LED (maybe your GPIO driver is what's
  broken). Generally, there's still something you can do which gives you one
  bit of debugging information---call `reboot()` or loop forever. If your
  program normally hangs, call `reboot()` to debug it; if your program reboots,
  you know you called `reboot()` before the bug. If your program normally
  crashes/reboots, run `while(1);` to debug it; if your program hands, you know
  you hit that loop before the bug.

## Iterate

You ran your experiment, and you found out something. Hopefully that something
was "my prediction was correct, and I found the bug", but more often than not
it'll be that your first prediction wasn't right. Now go back and make a new
prediction, using the information you've learned, and try again.

This might seem like a tedious process, and to some degree it is. However,
this is really the only foolproof way to find a bug; even if you ask us for
help, all we're going to be able to do is run through this process with you.
