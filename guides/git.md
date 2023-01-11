# Using Git

## GitHub login

Follow GitHub's [instructions](https://docs.github.com/en/github/authenticating-to-github/connecting-to-github-with-ssh) to set up SSH auth with GitHub.  This will make it much faster to pull new commits (you don't have to type your password every time).

## Cloning the Repo
To clone the 140e repo, run:
```sh
git clone git@github.com:dddrrreee/cs140e-21spr.git
```

This will create a new folder called "cs140e-21spr".

## Saving your work
Git has a concept of a "staging area"; this area includes all the changes which will be saved in a commit.  To save changes, you must first add them to the staging area.

To see your current repository status, run:
```sh
git status
```
This shows you the list of changed files.  Some will be marked as "Changes to be committed"; these are the ones in the staging area.  Others will be marked "Changes not staged for commit"; these won't be saved.

### Adding files to the staging area
For every file there you want to stage, run:
```sh
git add path/to/file
```
To add every changed and new file at once, run:
```sh
git add -A
```

### Removing files from the staging area
Run:
```sh
git restore --staged path/to/file
```
to remove a file from the staging area.  This won't change the file on your hard disk.

### Creating a commit
Run:
```sh
git commit -m "your commit message"
```
Commit messages should theoretically be succinct but descriptive.  For this class, you can do whatever you want.

### Amending a commit
Forgot to include some changes?  You can stage them as usual, then run:
```sh
git commit --amend
```
to add them to the latest commit.  Note that this only works for the most recent commit; there's a more complicated way to amend changes onto older commits, but at that point you should really just create a new commit.

### Undoing a commit
Committed too much?  Undo your last commit using:
```sh
git reset --soft HEAD^
```
This won't affect the files on disk.

## Synchronizing
Git has many different ways of synchronizing commits: email, a central server, printouts sent over carrier pigeon, etc.  For this class, we'll be pulling our commits from a central server (GitHub).

### Getting (pulling) our changes
First, commit (or stash) your changes.  This is important, you can't pull while you have uncommited changes.

To find new changes from the remote (GitHub), run:
```sh
git fetch
```
Note that this doesn't change anything on disk!  You can actually run this whenever you want, whether or not you have uncommitted changes; it just looks for new changes on GitHub, and pulls them into a hidden branch called `origin/main`.  However, all your work is on the `main` branch, so just running this command doesn't really do anything; you need to merge the changes into your local copy.  To do so, run:
```sh
git merge origin/main
```
This will attempt to automatically resolve every conflict between your local changes and the remote changes.  Most of the time it's successful, and it'll create a "merge commit" in your local copy which includes the remote changes.  However, on occasion, it may not be able to figure things out.  In that case it'll go into a state called a "merge conflict" and ask you to fix things manually.

You can run `git fetch && git merge origin/main` quickly by using `git pull`.

#### Fixing merge conflicts
First, run `git status` to figure out which files need to be fixed.  For every file labelled as "both modified", open it and look for "conflict markers".  These look like:
```
Common, non-conflicted line.
<<<<<<< HEAD
A change made locally.
=======
A change made remotely.
>>>>>>> origin/main
Another non-conflicted line.
```
In this case, the "conflict marker" shows you both versions of the file.  It's up to you to figure out which version to keep, and to make any other changes necessary.  Edit the file normally until it's in the state you want it to be, such as:
```
Common, non-conflicted line.
A change made locally and remotely.
Another non-conflicted line.
```
Then add the file to your staging area normally (`git add path/to/file`).  When you've done this for every file, run:
```sh
git commit -m "a message describing what you did"
```
The new changes from our repo should now be merged into yours.

### Pushing
If you have a remote repository with push access (e.g., a backup repository to store your code privately), you can run:
```sh
git push
```
to copy your changes to the server.

## Using the stash
Git maintains a stack of temporary commits if you want to really quickly save your changes temporarily (e.g., if you need to pull some new commits but don't want to commit your changes yet).

To push onto the stack, add your changes to the staging area, then run:
```sh
git stash
```

To pop from the stack:
```sh
git stash pop
```

## Backing up to another repo

You should definitely back up your work somewhere--having only your local copy is a recipe for disaster if your hard drive fails, you spill your coffee, etc.  The simplest way is to put your repo in your Google Drive or Dropbox folders, but a more advanced way is to back it up to another Git repo on GitHub.

1. Create a new repo on GitHub. [See here](https://docs.github.com/en/github/getting-started-with-github/create-a-repo) for instructions.  Make sure the new repo is private, so other people can't see it.  **Do not** select "initialize this repository with a README".
2. Clone the repo to your computer.
3. In the repo, run: (credit to jqlin for posting this in the group)
```sh
git remote add upstream https://github.com/dddrrreee/cs140e-21spr.git
git pull --allow-unrelated-histories upstream main
```
4. Whenever you want to get new changes from the class repo, run:
```sh
git pull upstream
```
5. Whenever you want to save your changes to your own github repository, run:
```sh
git push
```

## Branches
Git has a concept of "branches", which let you have parallel development processes and switch between them.  You've already used two branches--`main` and `origin/main`--but you can create as many as you want.

By convention, branches with a `/` in the name sometimes refer to branches whose "primary" copy is remote; for example, `origin/main` is the `main` branch on the `origin` server, which in this case is GitHub.

To list all your local branches, run:
```sh
git branch
```
The branch you're on will be marked with an asterisk.

To create a new local branch, run:
```sh
git checkout -b <branch name>
```
This creates a new local branch based on whatever branch you were previously on.

To switch branches, run:
```sh
git checkout <branch name>
```
Note that you'll have to stash or commit any changes to do this.

Technically, branches aren't in any way special--they're just names for commits.  Your git commit history actually forms a directed graph, and branches represent a "leaf" node.  In practice, it usually suffices to think of branches as linear commit histories which "branched" off from the `main` branch at some point in the past.  Merge commits are special in that they're the only nodes in the graph with multiple ancestors; they pull changes from one branch into the history of another.  You'll likely end up with a setup like this:
```
origin/main ----- create lab 0 ------- create lab 1 ------------- create lab 2 --------...
                                             \                          \
main ------------ create lab 0 --- lab 0 ---- merge lab 1 --- lab 1 --- merge lab 2 ---...
```
Except much messier, because git histories always end up a mess somehow.  For this class don't worry too much about branches and messy histories; if you ever need to recover an older version of a file and don't know how, contact us for help.

## Resetting the repo
If you want to reset your repo to the last commit, run `git` with the arguments `reset` and `--hard`.  This **will** erase any changes you've made since the last commit.

## Recovering old files
1. If you run `git log`, you can see your commit history.  Each commit has a hash, which looks like a 40-character hexadecimal string; identify the commit hash of the commit in which the correct version of your file is saved.
2. If you just want to look at the file, run `git checkout <hash>`; you can get back to the current version by running `git checkout <branch name>`.
3. If you want to reset a file to the state it was in at that time, run `git checkout <hash> <filename>`.  This will overwrite the file with the older version.
