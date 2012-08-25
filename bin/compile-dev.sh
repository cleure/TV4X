#!/bin/sh

CC="gcc"
CFLAGS="-Wall -g -O3 -std=c99 -I./include -I/opt/local/include -funroll-loops -ffast-math -msse2 -fomit-frame-pointer -finline-functions"
LIBS="/opt/local/lib/libpng.a /opt/local/lib/libz.a"
SOURCES=`find src -type f -name "*.c"`
TESTS=`find tests -type f -name "*.c"`

for i in $SOURCES
do
    obj=`echo $i | sed -e s/".c$"/".o"/`
    echo "Compiling $i"
    $CC $CFLAGS $i -c -o $obj
done

OBJECTS=`find src -type f -name "*.o"`
for i in $TESTS
do
    name=`echo $i | cut -d\. -f1`
    echo "Building $name"
    $CC $CFLAGS $OBJECTS $LIBS $i -o $name
done
