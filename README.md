TV4X
====

TV4X is a scaling filter, that emulates the look of CRT TVs. The main codebase is written in C, as per the C99 standard, and a framework for fast prototyping (written in Python) is also included.

TV4X currently includes two filters. A fast 2x scaling filter, and a slower 4x scaling filter.

Compiling
=========

Run bin/compile.sh

Build requires libz and libpng to compile/link.

Once the build is complete, you can process PNG files with the program "tests/test-filter", which will save the processed file to "out.png". Several 240p test images are included in the input-images folder. Example:

cmake .
make
./test-filter input-images/castlevania4_1.png
OR
./test-filter2 input-images/castlevania4_1.png

Performance
===========

The scaling filter performs rather well for the amount of work it does, however, there is a lot of room for improvement:

- The YIQ processing code does a lot of floating point math. It might be worth the effort to pre-process the initial conversion from RGB to YIQ.

TODO List
=========

- Refactor tv4x.c, so its more like tv2x.c (can take any input format, and outputs to the same format)
- SNES HiRes Support
- Perhaps YIQ math could be simplified?
- Explore ways to make tv4x_process_line branchless.
- Experiment with 3x scaling.
- See if its possible to implement YIQ with integer math.
