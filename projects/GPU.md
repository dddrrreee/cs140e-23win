# Raspberry Pi GPU Info

A few people mentioned doing final projects on the GPU, so here's an 
overview/some links:

## Architecture Overview

The BCM2835 SoC on the Raspberry Pi A+/0/0W has a [ARM1176JZF-S 700MHz][raspi] 
single-core CPU, as you're all well aware by now.  Interestingly, this is *not* 
the main processor on the SoC, even though we (and Linux) use it as such.  
Based on the way the system seems to be designed, the main processor is a 
dual-core [VideoCore IV CPU][vc4] running at 250MHz,[^vc4] which is called the 
"Vector Processing Unit" (VPU).  One of these cores is running ThreadX OS, and 
is what we talk to when we use the mailbox; the other is available for 
[user-supplied code.][mpi-code]  In addition, the SoC has an "quad processing 
unit", or [QPU][vc4-qpu], which is what we'd conventionally think of as the GPU 
(it's where shaders run, and is generally designed for working with images and 
vertices).

Note: you might have to undo the `config.txt` changes we made to increase CPU 
RAM, since those reduce GPU RAM.

## The Framebuffer

The simplest way to do (slow) graphics is the framebuffer, which you can access 
directly from the CPU after doing some mailbox setup.  Since cs107e already has 
[a thorough writeup][cs107e-fb], you can use that to get started if you just 
want HDMI output.

## The Vector Processing Units

The vector processing unit is interesting for working with vectors (obviously), 
and SIMD in general.  It's not as fast as the QPUs, but is much saner to 
program for.  There's an unofficial [Programmer's Guide][vc4-programming] 
available, a [WIP compiler][vc4-gcc], and a seemingly-stable 
[assembler][vc4-asm] which supports the VideoCore IV.  You can execute code 
using the [Mailbox Property Interface][^mpi-code] from the low-level hacks lab.  
You can theoretically talk to the core running ThreadX and request it to do 
more interesting things; let us know if you find documentation on this.

## The Quad Processing Units

From the [Broadcom Docs][bcm-qpu]:

    For all intents and purposes the QPU can be regarded as a 16-way 32-bit 
    SIMD processor with an instruction cycle time of four system clocks. The 
    latency of floating point ALU operations is accommodated within these four 
    clock cycles, giving single cycle operation from the programmer’s 
    perspective.

    [...]

    The QPUs are closely coupled to the 3D hardware specifically for fragment 
    shading, and for this purpose have special signaling instructions and 
    dedicated special-purpose internal registers. Although they are physically 
    located within, and closely coupled to the 3D system, the QPUs are also 
    capable of providing a general-purpose computation resource for non-3D 
    software, such as video codecs and ISP tasks.

The quad processing unit is interesting if you want to do 3D graphics (i.e., 
render lots of triangles), or if you can find some way to frame your 
calculations in terms of applying transformations and textures to vertices.  In 
a rare instance of openness in chip design, Broadcom has actually [published 
extensive documentation about the QPU][bcm-qpu].  It's still a lot (and very 
dense/technical/full of jargon and abbreviations), so I'd recommend taking a 
look at a few of these examples before getting started:

* some example [code][gpu-enable] to turn on the GPU
* [a reference][controllist] for how to build a control list that the GPU can 
  process
* [a description][pipeline] of the overall GPU pipeline, as well as 
  instructions on how to load data from the TMU (texture and memory lookup 
  unit) in assembly
* a working, albeit messy, [implementation][draw-triangle] of the code to draw 
  a triangle using the GPU.
* [an assembler][vc4asm] for QPU shaders
* the Mailbox Property Interface [docs][mpi] (which you've seen before), for 
  info on how to talk to the VideoCore memory allocator/manager and have it map 
  pages as CPU-readable

Programming the QPU is weird--it's so low level that you have to worry about 
things like "this operation doesn't produce results for three cycles, so I 
can't read its output register until then."  You have to frame everything in 
terms of an operation on vertices or on pixels for the QPU architecture to make 
any sense.

I (Akshay) did my cs107e final project on this, so I can probably help explain 
some of these things in more detail/provide example code.  Surprisingly, I have 
yet to notice any bugs with the QPU docs from Broadcom, but it's so large that 
you may well run into some.



[raspi]: https://www.raspberrypi.com/documentation/computers/processors.html
[vc4]: https://github.com/hermanhermitage/videocoreiv
[mpi-code]: 
https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface#execute-code
[vc4-qpu]: https://github.com/hermanhermitage/videocoreiv-qpu
[vc4-programming]: https://github.com/hermanhermitage/videocoreiv/wiki/VideoCore-IV-Programmers-Manual
[vc4-gcc]: https://github.com/itszor/vc4-toolchain
[vc4-asm]: http://sun.hasenbraten.de/vasm/
[cs107e-fb]: http://cs107e.github.io/assignments/assign6/
[bcm-qpu]: https://docs.broadcom.com/doc/12358545
[gpu-enable]: https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=187867
[controllist]: https://github.com/PeterLemon/RaspberryPi/tree/master/V3D/ControlList
[pipeline]: http://latchup.blogspot.com/2016/02/life-of-triangle.html
[draw-triangle]: https://github.com/phire/hackdriver/blob/master/test.cpp
[vc4asm]: https://github.com/maazl/vc4asm
[mpi]: https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface 
