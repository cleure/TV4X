TV4X
====

TV4X is a scaling filter, that emulates the look of CRT TVs. The main codebase is written in C, as per the C99 standard, and a framework for fast prototyping (written in Python) is also included.

The main tree currently takes RGB24 input, and produces RGB24 output. Work is currently underway to make it format agnostic.

Compiling
=========

Run bin/compile.sh

Build requires libz and libpng to compile/link.

Once the build is complete, you can process PNG files with the program "tests/test-filter", which will save the processed file to "out.png". Several 240p test images are included in the input-images folder. Example:

bin/compile.sh
tests/test-filter input-images/sonic2_2.png
open out.png

Performance
===========

The scaling filter performs rather well for the amount of work it does, however, there is a lot of room for improvement:

- The YIQ processing code does a lot of floating point math. It might be worth the
      effort to pre-process the initial conversion from RGB to YIQ.
    
- Most of the work done in tv4x_process_line() is doing random lookups on pre-processed
      data, and copying that data to output pointers. It may be possible to further optimize
      this code by using data structures that are more CPU cache friendly, as random lookups
      plus copy is where the biggest bottleneck is.


TODO List
=========

- Add something to allow certain features to be turned on/off, so that performance can be tuned to the underlying hardware.
    - YIQ Processing
        - YIQ Blur
        - YIQ Event Burst (Average/Simple/Off)
        - Luma/Chroma
        - Scale RGB up/down
- Rename external facing data types and functions, so they're all prefixed with "tv4x_".
- Experiment with 2x and 3x scaling.
