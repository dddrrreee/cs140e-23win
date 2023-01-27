-------------------------------------------------------------------------
### Lab extensions

There's a lot more you can do.  We will be doing a bunch of this later
in the class:

  - Save state onto the stack itself.

  - Make versions of the `libpi` `delay` routines that use your function rather than
  busy wait.  Make sure to check if threads are enabled in `rpi_yield()`!

  - Have a `sleep_until_us(us)` to put the current thread to sleep with a
  somewhat hard-deadline that it is woken up in `usec` micro-seconds.  Keep
  track of your cumulative error to see how well you're scheduling is doing.

  - Tune your threads package to be fast.  In particular, you can avoid
  context switching when a thread is run for the first time (there is
  no context to load) and you don't thave to save state when it exits
  (it's done).  You can also try tuning the interrupt handling code,
  since it is at least 2x slower than optimal.  (I'm embarrassed, but
  time = short.  Next year!)

   - See how many PWM threads you can run in `4-yield-test`.

   - One cute simple trick is we can emulate preemptive threading by yielding on every function exit. Do this. See 0-tracing-fn for an example for how to run a specific function on every code entry.
