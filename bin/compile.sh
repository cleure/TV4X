#!/bin/sh

CC="gcc"
CFLAGS="-Wall -g -O3 -std=c99 -I./include -funroll-loops -ffast-math -msse2 -fomit-frame-pointer -finline-functions"

OPTIONS="-DTV4X_YIQ_BLUR_ENABLED"

LIBS="-lpng -lz"
SOURCES=`find src -type f -name "*.c"`
TESTS=`find tests -type f -name "*.c"`

for i in $SOURCES
do
    obj=`echo $i | sed -e s/".c$"/".o"/`
    echo "Compiling $i"
    $CC $CFLAGS $OPTIONS $i -c -o $obj
done

OBJECTS=`find src -type f -name "*.o"`
for i in $TESTS
do
    name=`echo $i | cut -d\. -f1`
    echo "Building $name"
    $CC $CFLAGS $OPTIONS $OBJECTS $LIBS $i -o $name
done
