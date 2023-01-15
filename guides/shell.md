# Shell (Zach Yedidia)

The shell is a textual interface for running programs on your computer. The textual
interface is the most efficient and powerful way to control the computer because
it allows you to use programs and tools that don't have fancy graphical interfaces.

The default shell on your computer is probably either `bash` or `zsh` (for
MacOS).  To launch the shell, you need to open your terminal application (for
example, gnome-terminal, Terminal.app, or iTerm2).

## Commands

When your terminal opens, it starts running your default shell, and displays a
prompt:

```
user:~$
```

The prompt may display your username, and your shell's current working
directory (in this case `~`, which is an alias for your user's "home"
directory).

The shell will wait for you to give some input command, and will execute the
command when you press return. Once the command is done executing, the shell
will display a new prompt.

Print the current working directory:

```
user:~$ pwd
/home/user
user:~$
```

List the files in the current working directory:

```
user:~$ ls
Documents  Desktop  Downloads
user:~$
```

Most commands have optional or required arguments. These are given after the
command name, and customize the behavior of the command.  Arguments that begin
with `-` or `--` are often called flags or options and are used for customizing
what the command does.

```
user:~$ ls --help
Usage: ls [OPTION]... [FILE]...
List information about the FILEs (the current directory by default).
Sort entries alphabetically if none of -cftuvSUX nor --sort is specified.

Mandatory arguments to long options are mandatory for short options too.
  -a, --all                  do not ignore entries starting with .
  -A, --almost-all           do not list implied . and ..
      --author               with -l, print the author of each file
  -b, --escape               print C-style escapes for nongraphic characters
      --block-size=SIZE      with -l, scale sizes by SIZE when printing them;
                               e.g., '--block-size=M'; see SIZE format below
...[More documentation omitted]...
user:~$
```

I find `ls`'s `-l` flag very useful, as it gives a long listing with more
information and one line per file.

```
user:~$ ls -l
drwxr-xr-x  - user 10 Sep 12:37 Desktop
drwxr-xr-x  - user 10 Sep 12:37 Documents
drwxr-xr-x  - user 12 Sep 14:58 Downloads
```

In fact, I find `ls -l` so useful, I alias it to `ll` and only use that
instead of `ls` (see the configuration section).

We can also run `ls` with a directory as input to list all the files in
the given directory.

```
user:~$ ls -l Desktop
.rw-rw-r-- 20 user 16 Sep 20:06 myfile.txt
```

If you provide a file name, `ls` will display information about the given
file if it exists.

The `cd` command changes the current working directory:

```
user:~$ cd Desktop
user:~/Desktop$
```

There are many more commands installed supported by default. See
[here](http://www.mathcs.emory.edu/~valerie/courses/fall10/155/resources/unix_cheatsheet.html)
for a useful list. Use `man CMD` to get documentation about any command. Press
`q` to close the window after you are done.

You should use the shell for almost all operations you would otherwise use a
graphical file browser for. It will save you time!

NOTE: press Ctrl-C to cancel a command (e.g., if it is taking too long).

## PATH

When you run the `ls` command, the shell is executing the `ls` program, which
must be stored somewhere on your computer. There is a special shell variable
called `PATH` that stores a list of directories, each separated by a `:`. When
you run a command, the shell searches each directory for a file with the same
name as the command, and if a match is found, the shell executes that file.

We can display the value of `PATH` by using the `echo` command, and using the
`$` syntax to indicate that we are accessing a shell variable.

```
user:~$ echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
user:~$
```

The `ls` executable file is located in `/bin`, which on the `PATH` list.

```
user:~$ ls /bin/ls
/bin/ls
user:~$
```

Note that if `/bin/ls` did not exist, we would get a message like

```
ls: cannot access '/bin/ls': No such file or directory
```

Sometimes you aren't sure which executable the shell is using for a command. You can
use the `which` command to figure this out:

```
user:~$ which ls
/bin/ls
user:~$
```

When you install a new program, you should place the executable file for it in
a directory on the `PATH` list. Alternatively, you can modify the `PATH` list
to include a new directory, where the executable is located.

```
user:~$ export PATH=/path/to/new/dir:$PATH
```

This changes the value of `PATH`, prepending `/path/to/new/dir:` to it.

## Configuration

When the shell starts, it first runs a file of shell commands. This file is in your
home directory and is usually called `.bashrc` or `.zshrc` (depending on your shell).

Running `export PATH=...` only changes the `PATH` for the current shell session. If
we want to perform that export for every session, we could put that command in the
configuration file (e.g., `~/.bashrc`).

I also like to configure my prompt to exclude my username (since I already know
what that is), so I put this in my `~/.bashrc`

```
PS1='\w\$ '
```

or for a version with color:

```
PS1='\[\033[01;34m\]\w\[\033[00m\]\$ '
```

I alias `ls -l` to `ll` by putting the following in my `~/.bashrc`:

```
alias ll='ls -l'
```

## Multiple commands

Multiple commands can be run sequentially by placing a `;` between them:

```
~$ pwd; ls
/home/user
Documents  Desktop  Downloads
```

Every command terminates with an exit code (a number between 0 and 255). 0
indicates success and anything else indicates failure. Use `echo $?` to see the
exit code of the most recent command (the `true` and `false` programs return
success and failure exit codes respectively):

```
~$ true
~$ echo $?
0
~$ false
~$ echo $?
1
~$
```

Multiple commands can be run conditionally with `&&` and `||`. The `&&` operator
executes the next command if the previous one succeeded. The `||` operator executes
the next command if the previous one failed.

```
~$ true && pwd
/home/user
~$ true || pwd
~$ false && pwd
~$ false || pwd
/home/user
~$
```

## Redirection

Every command generates some (possibly empty) textual output. The command does
this by writing to two possible "streams" called standard out (stdout), and
standard error (stderr). Both are displayed in the same way by the shell, but
stderr is usually used for error messages instead of normal output.  Commands
can also read from a stream called standard input (stdin).

Redirection using the `>` operator lets you set the output stream of a command
to a file rather than stdout/stderr.

```
~$ ls > files.txt
~$ cat files.txt # cat can be used to display the contents of a file
Desktop
Documents
Downloads
```

Using `>` will redirect stdout. Use `2>` to redirect stderr.

When redirect, the output of a command at the shell is suppressed because it is
being written to a file. There is a special file called `/dev/null` that discards
the bytes written to it. You can redirect to this file to discard the output of
a command entirely.

```
~$ ls > /dev/null # output discarded
~$
```

The `<` operator can redirect stdin. By default `cat` will display what it receives
on stdin.

```
~$ cat < files.txt
Desktop
Documents
Downloads
~$
```

## Pipes

Pipes can be used to chain the stdout and stdin of multiple commands together.
The pipe operator is `|` (called "pipe").

Examples:

```
~$ cat names.txt
Alice
Waldo
Bob
Quinn
Eve
Mason
Mufasa
~$ cat names.txt | sort
Alice
Bob
Eve
Mason
Mufasa
Quinn
Waldo
```

Pipes are useful with the `tail`, `head`, and `grep` commands, along with many
others. Use `man CMD` to get documentation about any command. Press `q` to
close the window after you are done.
