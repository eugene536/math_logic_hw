#!/usr/bin/env bash
if [ "$1" == "" ]
then
    echo "empty argument"
elif [ "$1" == "clean" ]
then
    rm -rf main ../build
else
    mkdir ../build
    cd ../build
    cmake ../ && make
    cp main ../HW1
    cd ../HW1
    if [ "$1" != "build" ]
    then
        cat $1 | ./main | less
    fi
fi
