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
    cp main ../HW3
    cd ../HW3
    cd ../../task1/HW1/ && ./test.sh build && cd ../../task3/HW3
    cat $1 && echo -ne "\n\n" && cat $1 | ./main 2> /dev/null | ../../task1/HW1/task1 | tail -n 5 | less
fi

