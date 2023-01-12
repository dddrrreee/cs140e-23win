### Pre-lab

Reading deliverable:

- Read the paper `cs240lx-trusting-trust.annot.pdf` very carefully at
  least three times. You'll be implementing it. Do this first and
  thoroughly. You'll need it for thursday.
- Submit answers to the questions on
  [Gradescope](https://www.gradescope.com/courses/488143/assignments/2556045).

Coding deliverable: 0. Make sure you have some way to open at least two windows side by side
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

#### micro-SD card for next lab (lab 3)

For the next lab (lab 3) you'll be hooking up a pi and running it.
In order for this to work you will need to be able to write to a micro-SD
card on your laptop

1.  Get a micro-SD card reader or adaptor if its not built-in
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

    Pro tip: **_do not omit either a `sync` or some equivalant file
    manager eject action_** if you do, parts of the copied file(s)
    may in fact not be on the SD card. (Why: writes to stable storage
    are slow, so the OS tends to defer them.)
