# Setting up Ctags

## Background

Since we're going to be building a relatively complex piece of software, a 
little bit of help from your editor can go a long way towards making 
development easy.  While in normal circumstances you could use IDEs for this, 
IDEs aren't a great fit for OS development.  We have to deal with all sorts of 
really unusual things: cross-compiling from Intel to ARM, targeting baremetal, 
running our programs remotely and viewing the output over a serial port, 
calling from C into assembly and vice-versa, etc.  Trying to convince an IDE to 
understand what we're doing, even enough to provide minimal support, is an 
exercise in futility.

Luckily, there are a few tools which work great in the really weird environment 
we're going to be developing in.  Setting them up and learning how to use them 
will improve your OS development productivity immensely, and make debugging a 
lot easier (especially over Zoom).  Ctags is the most important of these tools, 
and you are **strongly** encouraged to set it up.

## Overview

[Ctags](https://en.wikipedia.org/wiki/Ctags) is a tool which indexes your code.  
While it's most commonly used with C, it supports many languages, and it has 
integrations with pretty much every editor (everything from Vim to Emacs to 
VSCode).

The benefits of using Ctags for this class are:
* It works across languages.  We're going to be doing a lot of crossing back 
  and forth between C and Assembly, and Ctags can handle both of them.  It can 
  even handle Markdown, so you can jump straight from a comment in your code to 
  the relevant section in the lab handout.
* It's really fast.  It should be able to reindex your entire codebase for this 
  class in a fraction of a second.
* It's standardized and well-supported.  Vim and Emacs have support built-in, 
  and plugins are available for most other modern editors.
* It makes development and debugging _way_ easier.  It powers features like 
  autocomplete and go-to-definition, and it works way better than whatever 
  heuristics are built into editors by default.
* Once set up, you don't need to worry about it; your editor and/or your 
  Makefile can rebuild your tags for you.

## Installation

There are a few different implementations of Ctags; the most modern is 
[Universal Ctags](http://ctags.io/).  To get started, download and install the 
binary:

- macOS: there's a Homebrew tap [on 
  GitHub](https://github.com/universal-ctags/homebrew-universal-ctags); follow 
  the instructions there.  If you don't have homebrew, you can install it from 
  [brew.sh](https://brew.sh/).
- Linux: install using your package manager; the package is probably called 
  "universal-ctags" or "ctags".  For our purposes, the older versions called 
  "exuberant-ctags" and just "ctags" will also work.

Once that's done, you can go to your cloned copy of the git repo and run `ctags 
-R`.  You should see a new file called "tags" in the directory; it's 
human-readable if you're curious about what's inside, but the contents are just 
metadata for your editor.  In general, you'll need to rebuild your tags file 
every so often to keep it up-to-date, but this is really easy to automate.

A simple way to automatically generate your tags is to add the following to 
your Makefiles:

```make
.PHONY: tags
tags:
        ctags -R .
```

Then run `make tags` to regenerate your tags file.  If you add "tags" to your 
"all" target, make will regenerate your tags every time you build.  For our 
class, you probably want to create a tags file at the root of the cs140e repo.

**Note:** emacs has a slightly different format for these files; run `ctags -eR` to generate a 
"TAGS" file compatible with emacs

## Usage

The way you use ctags will be slightly different depending on what editor you 
use.  I've given a brief overview for Vim/Neovim, Emacs, VSCode, and Sublime 
Text, but you should look up tutorials for your specific editor to get a full 
guide.

To regenerate your tags file from the command line, the simplest way is what 
was mentioned earlier: run `ctags -R .` from the root directory of the cs140e 
directory.  However, this may be suboptimal for a few reasons: since each of 
our labs will define its own functions, you may end up with a bunch of 
irrelevant definitions in your results; you may not be able to autocomplete 
some library functions; you probably don't want to cd back to the root every 
time you regenerate your tags, etc.

A more advanced way to generate your tags is:
1. run `ctags -R .` from your code directory (wherever your Makefile is)
2. run `ctags -aR $CS140E_PATH/libpi` to append the tags from your libpi 
   directory.
    - You can run the latter command again with other paths you want to 
      include.
This will limit your tags to only those functions which are relevant to the 
current lab.  This means you can open your editor in this directory, but still 
jump-to-definition into other directories if you need to.

You can configure your Makefile to do these steps automatically every time you build.

For Emacs, add the `-e` flag to every ctags invocation (e.g. `ctags -eR $CS140E_PATH/libpi`).

### Vim/Neovim

Vim has ctags support built in.
* To jump to definition, use `<C-]>` (jump to first definition).  Use `<C-t>` 
  to jump back.
    - This is also available as `:tag <tag> <RET>`
    - use `:tn` and `:tp` to jump to other definitions
* You can also use `g ]` (list all definitions) if a symbol is ambiguous.
* To autocomplete, use `<C-x><C-]>` or `<C-x><C-o>` (autocomplete tag and 
  omnicomplete, respectively).
* Use `:ts <symbol name> <RET>` to search for a symbol.
* You can set the `tags` setting to the paths of the tags files you want to 
  use.


More info:
- [Vim Wiki](https://vim.fandom.com/wiki/Browsing_programs_with_tags)
- [Vim/Ctags Tutorial](https://andrew.stwrt.ca/posts/vim-ctags/)
- type `:help tags` in vim to open the built-in documentation
- [Gutentags](https://github.com/ludovicchabant/vim-gutentags) automatically 
  updates your tags file every time you save.  It auto-detects git repositories 
  and creates a tags file for the entire repository.


### Emacs

Emacs also has built-in tags support.  As noted above, emacs uses a slightly 
different tags file format; the file is called "TAGS", and you can generate it 
using "ctags -eR".
* Use `M-.` to jump to definition.  Use `M-*` to jump back.
* Use `M-?` to find all references to the current symbol.

There are a few other commands you can find by searching for "xref" or "tags" 
inside Emacs.

More Info:
- [Emacs Wiki: Overview](https://www.emacswiki.org/emacs/EmacsTags)
- [Emacs Wiki: Automate tags](https://www.emacswiki.org/emacs/BuildTags)

### VSCode

VSCode doesn't support ctags natively, but there are a few plugins available.

[Exuberant 
CTags](https://marketplace.visualstudio.com/items?itemName=chriswheeldon.exuberant-ctags) 
provides go-to-definition.
* Use `F12` to go to definition.
* To reload your tags, press `Ctrl+Shift+P`, then search for the action 
  labelled "regenerate tags file".
* Autocomplete should work automatically.

[CTags Symbol 
Search](https://marketplace.visualstudio.com/items?itemName=valderman.ctagsymbols) 
provides symbol search.
* Use `Ctrl+P`, then type `#` to get a searchable list of every symbol in your 
  project.

### Sublime Text

Sublime Text 4 has a pretty good indexing engine for C built-in.  It doesn't 
always find files in `libpi` though, so we'd still recommend installing ctags.

[Sublime Ctags](https://packagecontrol.io/packages/CTags)
* Name your ctags file `.ctags` for it to get picked up automatically; you can 
  use a command like `ctags -R -f .tags` to do this (edit the `make tags` rule 
  in the Makefile).  Alternatively you can change the settings in the plugin to 
  look for files named `tags`.

Also recommended: [Sublime 
Terminal](https://packagecontrol.io/packages/Terminal) or similar to easily 
open a terminal in your current directory.

[Sublime LSP](https://packagecontrol.io/packages/LSP) (see below for more info)


## Optional Upgrade: Language Servers

A more advanced solution to developer tooling is language servers.  These are 
third-party binaries which communicate with your editor over a standard 
protocol (LSP - the language server protocol); they can provide much more 
accurate information than typical editor plugins, but at the cost of being more 
complicated to set up.  Unfortunately, they're not well-suited to OS 
development by default; it took me (Akshay) a few weeks to configure one in a 
sensible way in Vim, and I've never tried in other editors.  I've provided some 
information here if you're interested in setting one up; if done properly, it 
can be really helpful, but ctags is more likely to work overall.

Benefits of a Language Server:
* Contextual autocomplete and go to definition: since the server actively reads 
  and understands your code, it knows exactly what names are defined and where.
* Error checking: it'll highlight errors and warnings in your editor much like 
  an IDE would, which can save time while debugging.

More info, in case you have free time and want to explore:
- [ccls](https://github.com/MaskRay/ccls) is a good language server for C code
- [ccls Editor 
  Configuration](https://github.com/MaskRay/ccls/wiki/Editor-Configuration), 
  which includes most common editors
- [compiledb](https://github.com/nickdiego/compiledb) lets you translate 
  Makefiles into the format ccls expects

Some things to remember if you choose to do this route:
* Do this _after_ you have Ctags working.  That way, if your LSP setup breaks, 
  you still have Tags working.
* You need to tell your language server that we're compiling for baremetal 
  arm-none-eabi with no standard library, and where our custom library is.  The 
  easiest way is using a `compile_commands.json` file, which compiledb (see 
  above) can generate for you.
