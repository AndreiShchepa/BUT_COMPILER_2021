#!/bin/bash

set_dbg_cmake() {
    params="$params -DDEBUG_$1=ON"
}

set_compile_opts() {
	params="$params -DCOMPILE_$1=on"
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
		TESTS)
            set_compile_opts "$i"
            ;;
		INSTR)
            set_dbg_cmake "$i"
			;;
         *)
            ;;
    esac
done

declare -a arr=("rm -rf *"
                "cmake $params .."
                "make -j16"
                "./compiler <../$1"
                "cd ..")

if [ ! -d "build/" ]; then
    mkdir build
fi

cd build

if [[ `basename $PWD`  != "build" ]]; then
	exit 1
fi

for i in "${arr[@]}"
do
    eval "$i"
    if [ $? != 0 ]; then
        echo -e "\nERROR"
        exit 1
    fi
done
