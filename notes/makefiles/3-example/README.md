## 3-example

Now we use wildcards to automatically gather up all source and make it.
We avoid the use of dependencies by also using wildcards to get all
header files and stating that every source file depends on all header.

We also make the small change of parameterizing the program name (`NAME`)
that we are building.  

Everything should work the same!
