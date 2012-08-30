#!/bin/sh

CC="gcc"
CFLAGS="-Wall -g -O3 -std=c99 -I./include -I/opt/local/include -funroll-loops -ffast-math -msse2 -fomit-frame-pointer -finline-functions"

#OPTIONS="-DTV4X_YIQ_BLUR_ENABLED"
OPTIONS="-DTV4X_YIQ_BLUR_ENABLED"

LIBS="/opt/local/lib/libpng.a /opt/local/lib/libz.a"
SOURCES=`find src -type f -name "*.c"`
TESTS=`find tests -type f -name "*.c"`

for i in $SOURCES
do
    obj=`echo $i | sed -e s/".c$"/".o"/`
    echo "Compiling $i"
    $CC $CFLAGS $OPTIONS $i -c -o $obj
    if [ "$?" -ne "0" ]; then
        echo "Error Compiling: $i"
        #$CC $CFLAGS $OPTIONS $i -E
        exit $?
    fi
done

OBJECTS=`find src -type f -name "*.o"`
for i in $TESTS
do
    obj=`echo $i | sed -e s/".c$"/".o"/`
    name=`echo $i | cut -d\. -f1`
    echo "Building $name"
    $CC $CFLAGS $OPTIONS $i -c -o $obj
    if [ "$?" -ne "0" ]; then
        echo "Error Compiling: $i"
        exit $?
    fi
    
    $CC $CFLAGS $OPTIONS $OBJECTS $LIBS $obj -o $name
    if [ "$?" -ne "0" ]; then
        echo "Error Linking: $name"
        exit $?
    fi
done
