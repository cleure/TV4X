#!/bin/sh

for i in `find . -name "*.o"`;
do
    echo "Deleting: $i"
    rm $i
    if [ "$?" -ne "0" ]; then
        echo "Error Deleting: $i"
    fi
done
