We're writing out the prelab.  For the momemt, you should look at:
  
   0. Setting your `CS49N_2021_PATH` variable (see below).

   1. `00-hello`: checks that step 0 above worked.

   2. The sonar datasheet in the `3-sonar/docs` directory.

   3. [How to use a breadboard](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard/all).

Useful additional readings:

  - [A quick and dirty `Makefile` tutorial](http://nuclear.mutantstargoat.com/articles/make/).
  - [How to use a multimeter](https://learn.sparkfun.com/tutorials/how-to-use-a-multimeter/all).

-----------------------------------------------------------------
#### Setup your  CS49N_2021_PATH variable.

Similar to how you added your local `~/bin` directory to your shell's path
on the first lab, today you're going to edit your shell configuration file
(e.g., `.tcshrc`, `.bash_profile`, `.zshrc` etc) to set set an environment
variable `CS49N_2021_PATH` to contain where you have your repository.

If you change into your class repo directory and use `pwd` you can see 
its absolute path:

        % cd cs49n-21aut
        % pwd
        /home/engler/class/cs49n-21aut/

Then add an environment variable `CS49N_2021_PATH` to your  shell configuration
to point to this location.

E.g., for me, since I'm running `tcsh` I edit my `.tcshrc` file and add:

    setenv CS49N_2021_PATH /home/engler/class/cs49n-21aut/

To the end of it.  If I was using `bash` I'd do:

    export CS49N_2021_PATH=/home/engler/class/cs49n-21aut/

And, as before, don't forget to source your configuration file.  Again, for
`tcsh`: 

        % source ~/.tcshrc

As a simple test:

        % cd 3-sonar/00-hello
        % make
        % pi-install hello.bin

Should print:

        ... bunch of stuff ...
        TRACE:hello world from the pi
        DONE!!!

