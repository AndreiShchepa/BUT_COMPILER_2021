#!/bin/bash

set_dbg_cmake() {
    params="$params -DDEBUG_$1=on"
}

params=""

for i in "$@"; do
    case $i in
        RULES)
            set_dbg_cmake "$i"
            ;;
        SCANNER)
            set_dbg_cmake "$i"
            ;;
        EXPR)
            set_dbg_cmake "$i"
            ;;
         *)
            ;;
    esac
done

declare -a arr=("rm -rf *"
                "cmake $params .."
                "make"
                "./compiler <../$1"
                "cd ..")

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
