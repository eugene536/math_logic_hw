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
    cp main ../HW2
    cd ../HW2
    cat $1 | ./main | less
fi

