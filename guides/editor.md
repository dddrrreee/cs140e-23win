# Choosing an Editor

Given that the [Editor War](https://en.wikipedia.org/wiki/Editor_war) exists,
the choice of an editor is clearly not something that people take lightly.  
Different editors have different strengths and weaknesses.

For this class, any editor which lets you edit text files is theoretically good
enough; however, Vim or Emacs are probably your best options. You can get by
with VSCode or Atom, but they may not provide as much help. You should avoid
any sort of IDE (XCode, Eclipse, Visual Studio), since they're not well-suited
for OS development.

If you're open to learning a new editor, [Vim](https://www.vim.org/) is a good
choice. It's small and well-suited to C development. If you're using macOS or
Linux, you probably already have Vim installed, just type "vim" at your shell
prompt. More details (and a comparision to other editors) are below.

No matter what editor you choose, make sure you have autocomplete and
go-to-definition working in your editor, and make sure you know how to use
them. See [the Ctags guide](ctags.md) for more info.

Relevant xkcd: [https://xkcd.com/378/](https://xkcd.com/378/)

## Vim

Vim is a very versatile editor, and is installed on most UNIX systems by
default. There are two current versions: there's the traditional [Vim
8](https://www.vim.org/), and there's a fork called
[Neovim](https://neovim.io/). Both are pretty similar in functionality, and
most plugins work the same in either one; Neovim is usually slightly ahead in
terms of new features, but it doesn't really matter.

Benefits of Vim:

- small, fast
- UNIX philosophy: it does one thing (edit text files), and it does it well
- modal editing can be really powerful
- very configurable
- works in any terminal on any UNIXy computer

Downsides of Vim:

- really steep learning curve: there's really no other popular modal text
  editor, so it's a foreign paradigm for most
- configured in VimScript, which is slow/unintuitive
  - Neovim can be configured in Lua, which is more complicated but a better
    programming language

You can run a tutorial by running `vimtutor` from your terminal (Vim 8), or by
opening Neovim and typing `:Tutor`. If you haven't used Vim before, I'd
suggest doing the tutorial, then creating a file called ".vimrc" in your home
directory with the content:

```vim
syntax on      " Enable basic syntax highlighting
set number     " Show line numbers
set mouse+=a   " Enable mouse
```

You can get more information by typing `:help` after opening vim.

## Emacs

Emacs is another very popular editor. [GNU
Emacs](https://www.gnu.org/software/emacs/) is the most popular implementation,
and runs on pretty much every platform. Emacs is known for bundling every
possible program you could imagine into itself; it contains a web browser, a
mail client, a few games, a hex viewer, and a text editor.

Benefits of Emacs:

- comprehensive
- contains basically anything you could ever want
- highly configurable, and can be scripted in Emacs Lisp
- a more familiar interface than Vim, albeit with unusual keyboard shortcuts

Downsides of Emacs:

- some keyboard shortcuts are really complicated, and were designed for an
  older keyboard layout; in some cases they may cause RSIs (like carpal tunnel syndrome)
- large and somewhat slow; not usually a problem on newer systems
- limited in the terminal: it works, but it's really meant to be used in a GUI
  nowadays

## VSCode/Atom/Sublime/etc.

VSCode (and other modern editors) are very popular, but are generally far more
limited than Vim or Emacs for OS-style development. They'll probably work for
this class as long as you [set up Ctags](ctags.md), but they may be
inefficient. Of this set, [Sublime Text](https://www.sublimetext.com/) is
probably the best suited for this class on merit of being least IDE-like, but
we know many people in the past have preferred VSCode due to familiarity.

Benefits of VSCode et al.:

- really popular
- familiar editing paradigm (mouse and keyboard)
- standard keyboard shortcuts

Downsides of VSCode et al.:

- not designed for C development: you'll need to install several plugins to get
  up to speed, and you'll still need to keep a few terminal windows around
  anyway
- not well-integrated with UNIX
- (Atom and VSCode): large, slow, and memory-intensive; these are both Electron
  apps and therefore require almost the same resources as a Chrome window
- using the mouse may cause RSIs (like carpal tunnel), and is generally slower
  than using the keyboard
