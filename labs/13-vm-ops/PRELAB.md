## Prelab: vm coherence and operations.

Prelab is pretty simple: read chapter B2 of the armv6 document, focusing
on pages B2-16 til the end of the chapter.  More than any other document,
I would recommend printing this one out (we gave this out in class):

   - `12-pinned-vm/docs/armv6.b2-memory.annot.pdf`
   - And its companion [cheat sheet for B2](MEMORY-ORDER-cheatsheet.md).
     This has a useful, hopefully correct distillation of the rules.

There isn't a lot of prose, but you need to think carefully about why and
what you have to do.  Make a list of rules.   We will spend some initial
class time discussion going over this, which won't be very useful if
you are unfamiliar with the details.

IMO, these are the most subtle of the rules on the ARM (and on many
chips), so being able to reason about them means that everything else is
comparatively easy.  It also means you have a real grasp of how actual
reality works and looks like; most implementors have no idea.
