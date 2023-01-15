[Notes from Zach Yedidia]

# Basic types

The computer memory stores bytes. To make these bytes more useful, we associate
them with some context, called a type. Every type has a corresponding size in
bytes. Every type also has an "alignment." This means that a value with type
`T` must be stored at an address that is a multiple of `T`'s alignment.
Alignment is used for convenience and efficiency, but sometimes results in
wasted space in the form of padding bytes.

Type     |           Size (64 bit) |  Alignment (64 bit)  |    Size (32 bit)  |   Alignment (32 bit) |
----------------- |  --------------|  -------------------  |   --------------   | ------------------- |
`char`           |       1        |          1          |          1         |        1               |
`short`          |       2        |          2          |          2         |        2               |
`int`            |       4        |          4          |          4         |        4               |
`long`           |       8        |          8          |          4         |        4               |
`long long`      |       8        |          8          |          8         |        8               |

# Pointers

Most values in a C program are stored in memory somewhere. An address is a
number that specifies a position in the computer's memory. In order for an
address to be useful, we must know what type of value is stored at that
address.  A value with a pointer type, `T*`, stores the address of a value with
type `T`.

On a 32-bit architecture the size and alignment of `T*` is 4, and on a 64-bit
architecture it is 8.

Address      |  Value  |
--------     |  ------ |
0x2021eca8   |  16      |
0x2021ecac   |  5      |
0x2021ecb0   |  -3      |
0x2021ecb4   |  9      |

Suppose we have a pointer:

```c
int* ptr = (int*) 0x2021eca8;
```

The operation `*p` "dereferences" the pointer `p`: `*ptr` is the value stored
at the address of `ptr`. In this case, `*ptr == 16`.

## Pointer arithmetic

In C, an array is simply a pointer. We use `&value` to get a pointer to `value`. Thus we have:

```c
ptr[0]  == 16
ptr[1]  == 5
ptr[2]  == -3
ptr[3]  == 9

&ptr[0] == (int*) 0x2021eca8
&ptr[1] == (int*) 0x2021ecac
&ptr[2] == (int*) 0x2021ecb0
&ptr[3] == (int*) 0x2021ecb4
```

We can also use pointer arithmetic, which is similar to addressing into the pointer "array."

```c
ptr + 0 == (int*) 0x2021eca8
ptr + 1 == (int*) 0x2021ecac
ptr + 2 == (int*) 0x2021ecb0
ptr + 3 == (int*) 0x2021ecb4

*(ptr + 0) == 16
*(ptr + 1) == 5
*(ptr + 2) == -3
*(ptr + 3) == 9
```

Note that when you add 1 to a pointer with type `T*`, the address is increased
by `sizeof(T)`. If you ever need to add 1 to the address of a pointer, you
should first cast the pointer to a `char*`, which ensures that `sizeof(T) ==
1`. On most compilers adding 1 to a pointer of type `void*` will also increase
the address by 1, but this is not guaranteed by the C spec.

In fact, for an array `T x[N]`, for `0 <= i <= N`, `&x[i] == x + i`. It is
illegal to form a pointer that points outside the object (not just dereference,
but *form*). In this case that would be `x + i` where `i < 0` or `i > N`. Note
that forming the pointer `x + N` is legal, but dereferencing is not.

Subtracting two pointers is valid as long as both pointers point within the
same object.

# More types

## Sized integers

Purpose: Use standard names for specific-sized integer types.

Key idea: Standard names for important integer types are provided by `#include
<stdint.h>`. These names include:

* `uint8_t`: Unsigned integer type containing exactly 8 bits. Equals unsigned
  char on every machine we care about.
* `int8_t`: Signed integer type containing exactly 8 bits. Equals signed char
  on every machine we care about.
* `uint16_t`, `int16_t`: Same for 16-bit integers. Usually equals unsigned
  short and short.
* `uint32_t`, `int32_t`: Same for 32-bit integers.
* `uint64_t`, `int64_t`: Same for 64-bit integers.
* `uintptr_t`, `intptr_t`: Integer types with the same size as pointer types.
  On machines with 32-bit addresses, such as the Pi 1, `uintptr_t` is the same
  type as `uint32_t`. On machines with 64-bit addresses, such as your computer
  (probably), `uintptr_t` is the same type as `uint64_t`.
* `size_t`: Unsigned integer type used to represent sizes of objects that can
  fit in memory. This is usually the same type as `uintptr_t`. Every sizeof(x)
  expression returns a value of type `size_t`.
* `off_t`: Unsigned integer type used to represent file sizes and positions
  within files. This might be bigger than `size_t`, since most machines can
  manipulate disk files that are larger than memory.
* `ssize_t`: Signed integer type with the same width as `size_t`.
* `ptrdiff_t`: Pointer subtraction expressions, such as `ptr1 - ptr2`, have
  this type. It is generally the same as `intptr_t`.

# Bitwise operators

Remember that every integer is just a bunch of bits. Bitwise operators let you
manipulate an integer via the bits rather than via traditional arithmetic
operators like `+` or `-`.

C supports the following bitwise operators that are of interest to us, for use
with integer values:

* `~a`: inverts all bits in `a` (complement).
* `a | b`: bitwise OR between all the bits in `a` and `b`.
* `a & b`: bitwise AND between all the bits in `a` and `b`.
* `a ^ b`: bitwise XOR between all the bits in `a` and `b`.
* `a << n`: shift all bits in `a` left by `n`, filling in new slots with 0s.
* `a >> n`: shift all bits in `a` right by `n`, filling in new slots with 0s.

## Set, clear, toggle, check

In the following cases, we have an integer `i` and we want to
perform some action on bit `n` within `i`.

Set bit:

```
i |= 1UL << n;
```

Clear bit:

```
i &= ~(1UL << n);
```

Toggle bit:

```
i ^= 1UL << n;
```

Check bit:

```
bit = (i >> n) & 1UL;
```

## Multi-bit changes

We can also change multiple contiguous bits at once.

Set 6 bits to 0 starting with bit `n`:

```
i &= ~(0xfff << n)
```

Set 6 bits to 1 starting with bit `n`:

```
i |= 0xfff << n
```

# Sources

* https://cs61.seas.harvard.edu/wiki/2017/Patterns
* https://cs61.seas.harvard.edu/wiki/2017/Datarep2
* https://cs61.seas.harvard.edu/wiki/2017/Section1
* https://pdos.csail.mit.edu/6.828/2014/readings/pointers.pdf

