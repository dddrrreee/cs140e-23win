Using the PRELAB as an example, modify this code to trace `GET32` and
`PUT32` and use `printk` to emit the result.

NOTE:

  - `printk` uses GPIO and hence, calls `PUT32` and `GET32`. Thus,
    to prevent an infinite loop while tracing, before calling `printk`
    your system should set a variable that lets it know it should skip
    any subsequent calls.  When the `printk` is done, you would reset
    the variable.

  - If you want to trace `printk` you would have to add a buffering
    mode, where instead of immediately printing all `PUT` and `GET`
    calls during tracing you would instead add them to a sequential list
    (a "log").  When `trace_stop()` is called, you would iterate over
    this list, printing it.

A description of how to run the tests is in [tests/README.md](tests/README.md).
