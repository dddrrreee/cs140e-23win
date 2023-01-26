## Prelab for threads

Gradescope: [Prelab 6](https://www.gradescope.com/courses/488143/assignments/2609703/)

***[This will be added to over the w/e]***

In this lab, you'll write a simple non-preemptive thread package.

We'll have to save and restore registers for thread context switching
and know how procedure calls work at the assembly level:

Some useful introduction (or review) reading for threads:
  1. [Lecture 2 and Lecture 3](https://www.scs.stanford.edu/18wi-cs140/notes/) from Mazieres'
     CS140 lectures.
  2. [Threads intro](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
     and [Thread API](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf)
     from the useful [Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/#book-chapters)
     textbook.

Deliverables:
  1. Look through the `9-threads/code/rpi-thread.[ch]` header and skeleton code
     so you see what you will be implementing.
  2. Read about threads carefully, especially the API chapter above.  
  3. Figure out these puzzles: what do we have to do to context
     switch from one thread to another?  How to switch from "normal
     execution" to the first thread?  How to handle the case where a
     thread running `foo()` does not call `rpi_thread_exit` explicitly
     and simply returns back to its (non-existant) caller?
