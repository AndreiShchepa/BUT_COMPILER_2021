#!/bin/bash

declare -a arr=("cmake .."
                "make"
                "./compiler <../$1")

if [ ! -d "build/" ]; then
    mkdir build
fi

cd build

for i in "${arr[@]}"
do
    eval "$i"
    if [ $? != 0 ]; then
        echo -e "\nSOME ERROR"
        exit 1
    fi
done
