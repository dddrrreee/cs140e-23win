### Pre-lab

Reading deliverable:

- Read the paper
  [`cs240lx-trusting-trust.annot.pdf`](./cs240lx-trusting-trust.annot.pdf) very
  carefully at least three times. You'll be implementing it. Do this first and
  thoroughly. You'll need it for Thursday.
- Do the [prelab
  assignment](https://www.gradescope.com/courses/488143/assignments/2556045) on
  Gradescope (you should have been automatically added, if not let us know).
  The process in the paper looks misleadingly simple at first, so hopefully the
  questions will help locate the tricky parts before you implement them
  yourself.

Coding deliverable: Make sure you have some way to open at least two windows side by side
on your laptop so you can edit code in one and compile in the other.
Among other things, this makes fixing compile errors much easier.

1. Make sure you have some way to click on identifiers in code so you
   can immediately see their definition. In `emacs` you can use "etags".
   In `vim` or `vi` "ctags". If you get stuck or figure out an easy trick
   please post in the newsgroup!
2. Check-out the class git repo.
3. Figure out how to add a local `~/bin` directory in your home directory
   to your shell's `PATH` variable.
4. Have a way to mount a micro-SD card on your laptop. You may need
   an adaptor.
5. Make sure you know how to use common unix commands such as: `ls`,
   `mkdir`, `cd`, `pwd` and `grep`.

---

### git and checking out the class repo

1.  If you don't have `git`, install it on your laptop. We use `git`
    to push all assignments and code to you.

    MacOS seems to ship with `git`, but you might need to install the
    xcode command line tools with:

        % xcode-select --install

    Ubuntu:

        % sudo apt install git

2.  Use `git` to clone (check out) the class repository:

        % git clone git@github.com:dddrrreee/cs140e-23win.git

    Not recommended: If you don't want to install a key on github, use:

        % git clone https://github.com/dddrrreee/cs140e-23win.git

    Pro tip: run your `git clone` command in some sensible directory
    (e.g., `~/class/`) and remember where you checked your repository
    out to! Also, know how to find it. We had a surprising (to me)
    number of people lose work last year because they did not do this.

    Pro tip: **_Do not download the class repository directly as a
    .zip file_**! Direct download will be both tedious and overwrite
    any local changes you do later.

---

#### Part 1: micro-SD card stuff

In order to run code on the pi, you will need to be able to write to a
micro-SD card on your laptop:

1.  Get/bring a micro-SD card reader or adaptor if its not built-in
    to your laptop. The [CS107E guide on SD
    cards](http://cs107e.github.io/guides) is a great description
    (with pictures!).

2.  Figure out where your SD card is mounted (usually on MacOS it is in
    `/Volumes` and on linux in `/media/yourusername/`, some newer linuxes
    might put it in `/run/media/yourusername/`). Figure out
    how to copy files to the SD card from the command line using
    `cp`, which is much much faster and better than using a gui (e.g.,
    you can put it in a
    `Makefile`, or use your shell to redo a command). For me,

          % cp kernel.img /media/engler/0330-444/
          % sync

    will copy the file in `kernel.img` in the current directory to the
    top level directory of the mounted SD card, the `sync` command forces
    the OS to flush out all dirty blocks to all stable media (SD card,
    flash drive, etc). At this point you can pull the card out.

    Pro tip: **_do not omit either a `sync` or some equivalent file
    manager eject action_** if you do, parts of the copied file(s)
    may in fact not be on the SD card. (Why: writes to stable storage
    are slow, so the OS tends to defer them.)

---

### Part 2: change your `PATH` variable

To save you time, typing and mistakes: tell your shell to look for
executable programs in a `bin` directory located in your home directory.

1.  For whatever shell (`tcsh`, `bash`, etc) you are using, figure
    out how to edit your `PATH` variable so that you can install binary
    programs in a local `~/bin` directory and not have to constantly
    type out the path to them. E.g.,

          % cd ~          # change to your home dir
          % mkdir bin     # make a bin directory
          % cd bin        # cd into it
          % pwd           # get the absolute path
          /home/engler/bin

    To figure out your shell, you can typically check in `/etc/password`:

          % grep engler /etc/passwd
          engler:x:1000:1000:engler,,,:/home/engler:/usr/bin/tcsh

    Since I use `tcsh`, to add `/home/engler/bin` to my `path` I would
    edit my `~/.tcshrc` file and change:

         set path = ( $path  )

    To:

         set path = ( $path /home/engler/usr/bin )

    and then tell the shell to re-scan the paths as follows:

          % source ~/.tcshrc

    Note, you have to run `source` in every open window: it may be
    easier to just logout and log back in.

    For MacOS users using `bash`, put the path to your `~/bin` directory
    in a `.bashrc` file in your home directory. Just run:

          % vim ~/.bashrc

    When you add your path, make sure you include you default path or
    you may mess up your terminal. This looks like:

          export PATH="$PATH:your/path/here:your/other/path/here"

    and source it the same way as the `.tcshrc`. The instructions for `zsh`
    (the default on some newer Macs) are the same as for `bash`, but edit
    `~/.zshrc` instead.

---

#### Tags

Figure out how to use tags. Both `emacs` and `vim` support "tags",
which use keyboard shortcuts to essentially treat identifiers
as urls and navigate to the definitions of structures,
functions, variables. This makes understanding code
**_much much_** faster and easier. [A description for
`vim`](https://vim.fandom.com/wiki/Browsing_programs_with_tags).

See [the ctags guide](../../guides/ctags.md) for more detailed info.

If you're using VSCode, don't trust the built-in autocomplete and
go-to-definition; it doesn't work on OS code like we'll be writing. Set up tags
inside VSCode to avoid a headache later. Trying to use control-f instead of
go-to-definition is slow and error-prone, especially if we're trying to help
you debug.

---

#### Nice-haves: Extra, very useful things.

If you're not super familiar with UNIX commands, we recommend skimming through
[MIT's "The Missing Semester"](https://missing.csail.mit.edu/) lecture notes.  
You don't need to be an expert by any means, but being able to do things from
the shell will make this class easier.

Commands you should be familiar with (not an exhaustive list):

- `man` (this is both the UNIX and C manual; you'll need this for sure)
- `echo`, `printf`
- `ls`
- `cat`, `less`
- `cd`, `pushd`, `popd`
- `grep`, `find`, `sed`

You don't need these for the first lab, but they will make
your Stanford journey much easier:

- Figure out how to "tab complete" in your shell. This gives you
  a fast(er) way to list out files and run things. We'll show this,
  but you can google.

- Longer term: eliminate as much mouse usage as you can; **_especially
  eliminate screen switching_**. Ideally you'll have your code in one
  window, the compilation in another window on the same screen and
  switch back and forth between these windows without lifting your
  hands off the keyboard. You should definitely not be switching
  screens for such tasks.

  The single biggest productivity slowdown I see in stanford undergrads
  (grads too) is the slow, laborious use of taking their hand off
  a keyboard, moving a mouse, clicking, switching screens, clicking
  on something else, switching back, etc. You don't realize how
  much this will cripple you until you watch someone who writes code
  well and compare their fluent flow to your plodding agrarian
  lifestyle. Such switching screens means you lose context
  (which makes fixing even trivial compilation errors painful),
  you move slowly, and in many different ways you will distrupt your
  programming flow state, which is basically the only real way you
  can get significant, effective work done. Any time you move the
  mouse, you're not doing work. Do what you can to stop this.

  Vim and Emacs are good, popular, well-supported editors which don't use
  the mouse. They integrate with all the tools we'll use in this class by
  default, and you can automate anything you do often. See [the editor
  guide](../../guides/editor.md).

  If you want to use a GUI-based editor like Visual Studio Code, please
  learn the keyboard shortcuts for common tasks (compile, go to definition,
  search, etc.). The mouse may be fine for things you do once in a while,
  but anything you're doing more than once per hour should really not
  require the mouse. (Seriously, if you find yourself doing the same thing
  over and over, try to think of a way to automate it. Don't settle for an
  inefficient process just because it's the default.)

- Install `gdb` if you can --- it makes things easier in other classes
  (e.g., you can do a backtrace after a crash to figure out where the
  code was) and for this class let you answer simple code questions
  quickly. For example, what is the decimal value of `01010101011`?

       % gdb
       (gdb) p 0b01010101011
       $1 = 683

  Easy.

---

### Acknowledgements

Thanks to Ian (who took the class in 2021) for helping greatly improve the
writeup.
