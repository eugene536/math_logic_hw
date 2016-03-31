#!/bin/bash

for i in {1..11}
do
    cat "HW4/incorrect$i.in" > ./resources/in 
    ./build/Task 2> /dev/null
    if [ $? -ne 1 ]; then
        echo "fail =("
        exit 1
    else
        echo "incorrect$i"
    fi
done

for i in 1 2 5 6 7 8 9 10 11 12 13 14 15
do
    cat "HW4/correct$i.in" > ./resources/in 
    ./build/Task 2> /dev/null
    if [ $? -ne 0 ]; then
        echo "fail =("
        exit 1
    else
        echo "correct$i"
    fi
done
