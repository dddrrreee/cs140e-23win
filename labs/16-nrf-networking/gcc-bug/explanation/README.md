An interesting "bug"---gcc seems to emit an invalid unaligned access to the
struct in `foo`:
```
00000000 <foo>:
   0:	e3a03000 	mov	r3, #0
   4:	e1c030b1 	strh	r3, [r0, #1]
   8:	e12fff1e 	bx	lr
```

Even though the struct has size 2, it's being stored at offset 1.  However,
`strh` seems to require an offset of 0 or 2 to work.  On the Raspberry Pi, this
manifests as writing to the wrong address.

The (mis)alignment of the struct is actually *required* by the ARM calling
convention [1].  Section 5.3 of the calling convention says:
  
* The alignment of an aggregate shall be the alignment of its most-aligned
  component.
* The size of an aggregate shall be the smallest multiple of its alignment that
  is sufficient to hold all of its members when they are laid out according to
  these rules.

As the struct only contains bytes (with natural alignment 1), the alignment of
the struct itself inherits the alignment 1.  Therefore, putting it at an
"unaligned" offset is not only allowed, but necessary for an ARM C compiler.
When the struct instead contains a halfword/short (with natural alignment 2),
as in `bar`, the struct changes to the expected alignment of 2.

```
0000000c <bar>:
   c:	e3a03000 	mov	r3, #0
  10:	e1c030b2 	strh	r3, [r0, #2]
  14:	e12fff1e 	bx	lr
```

The second question is why GCC tried to emit an unaligned access rather than
doing two separate one-byte accesses when writing to the struct.  This is
because, according to ARMv6, this is completely valid: section A2.8 of the
ARMv6 document [2] explains the rules around unaligned accesses on ARMv6, and
this is a qualifying case.  This means `strh` should be able to use an offset
of 1.

Given that this is valid according to both the calling convention and ARMv6,
why doesn't this work on the Pi?  The trouble is that ARM1176 defaults to
backwards-compatibility mode with architectures that *didn't* support unaligned
accesses, and by default truncates the bottom bits of a halfword store.  This
is described in Chapter 4 of the ARM1176 doc; page 4-2 states:

* CP15 Register c1 has a U bit that enables unaligned support. This bit was
  specified as zero in previous architectures, and resets to zero for
  legacy-mode compatibility.

Since libpi never sets up this bit, but gcc assumes it's set [4], we get a
"bug" because the hardware's behavior differs from gcc's.

There are two solutions to this:
1. What we did, which is give gcc the `-mno-unaligned-access` flag to tell it
   we're not setting the U bit.
2. Enabling the U bit to make unaligned accesses work the way gcc expects.

[1]: https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst
[2]: https://raw.githubusercontent.com/dddrrreee/cs240lx-22spr/main/docs/armv6.pdf
[3]: https://raw.githubusercontent.com/dddrrreee/cs140e-22win/main/docs/arm1176.pdf
[4]: https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/ARM-Options.html
