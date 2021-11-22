#!/bin/bash

set_dbg_cmake() {
    params="$params -DDEBUG_$1=ON"
}

set_compile_opts() {
	params="$params -DCOMPILE_$1=on"
}

params=""
execute=1
in=""
out=0

while [ "$#" -gt 0 ]; do
    case "$1" in
        "RULES")
            set_dbg_cmake "$1"
            ;;
        "SCANNER")
            set_dbg_cmake "$1"
            ;;
        "EXPR")
            set_dbg_cmake "$1"
            ;;
		"TESTS")
            set_compile_opts "$1"
            ;;
		"INSTR")
            set_dbg_cmake "$1"
			;;
		"-e0")
			execute=0
			;;
		*".tl")
		    in="$1"
			;;
        "--out")
            out=1
            ;;
    esac
    shift
done

declare -a arr=("rm -rf *"
                "cmake $params .."
                "make -j16"
                "./compiler <../$in"
                "cd ..")

if [ "$execute" -eq 0 ]; then
    unset "arr[3]"
fi

if [ ! -d "build/" ]; then
    mkdir build
fi

if [ "$out" -eq 1 ];then
    arr[3]="./compiler <../$in >../$in.out"
fi

cd build || exit 1

for i in "${arr[@]}"
do
#   echo "$i"
     eval "$i"
     if [ $? != 0 ]; then
         echo -e "\nERROR"
         exit 1
     fi
done
