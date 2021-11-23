#!/bin/bash
complete -c timedatectl -s h -l help -d 'Print a short help text and exit'

set_dbg_cmake() {
    params="$params -DDEBUG_$1=ON"
}

set_compile_opts() {
    params="$params -DCOMPILE_$1=on"
}

params=""            # params for cmake
in=""                # input_filename
exec=0               # compile || compile and execute
out=0                # will print output to "input_filename.ifjcode" and not into stdout
compile=0            # compile project
compile_to_lua=0     # compile code in tests_code_gen to lua code
compile_to_ifjcode=0 # compile code in tests_code_gen to lua code
valgrind=0           # run valgrind with input file
help=0
clean=0              # clean tests_code_gen directory, remove compiled codes

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
    "--exec")
        exec=1
        ;;
    *".tl")
        in="$1"
        ;;
    "--out")
        out=1
        ;;
    "--compile_to_lua")
        compile_to_lua=1
        ;;
    "--compile_to_ifjcode")
        compile_to_ifjcode=1
        ;;
    "--clean")
        clean=1
        ;;
    "--compile")
        compile=1
        ;;
    "--valgrind")
        valgrind=1
        ;;
    "--help")
        help=1
        ;;
    esac
    shift
done

if [ "$help" -eq 1 ]; then
    echo "./start                                       # does nothing"
    echo "./start --compile filename.tl --exec --out    # compile execute and stdout print to file_basename.ifjcode"
    echo "./start --compile filename.tl --exec          # compile execute and print to stdout"
    echo "./start --compile filename.tl                 # just compile"
    exit 0
fi

if [ ! -d "build/" ]; then
    mkdir build
fi

#  compile project
if [ "$compile" -eq 1 ] || [ "$exec" -eq 1 ]; then
    cd build || exit 1
    eval "rm -rf *"
    eval "cmake $params .."
    eval "make -j16"
    if [ "$exec" -eq 1 ] && [ "$out" -eq 1 ]; then
        eval "./compiler <../$in >../${in%.*}.ifjcode"
    elif [ "$exec" -eq 1 ]; then
        eval "./compiler <../$in"
    fi
    cd .. || exit 1
fi

# compile to lua all file in tests_code_gen
if [ "$compile_to_lua" -eq 1 ]; then
    cd tests_code_gen || exit 1
    for file in *.tl; do
        compile_cmd="tl gen \"$file\""
        eval "$compile_cmd" || exit 1
    done
    cd ..
fi

# compile to ifjcode all file in tests_code_gen
if [ "$compile_to_ifjcode" -eq 1 ]; then
    cd build || exit 1
    for file in ../tests_code_gen/*.tl; do
        compile_cmd="./compiler <${file} >${file%.*}.ifjcode"
        eval "$compile_cmd" || exit 1
    done
    cd .. || exit 1
fi

# remove from tests_code_gen compiled codes
if [ "$clean" -eq 1 ]; then
    cd tests_code_gen || exit 1
    rm *.lua
    rm *.ifjcode
    rm *.out
    cd ..
fi

# valgrind
if [ "$valgrind" -eq 1 ]; then
    valgrind_cmd="valgrind  --tool=memcheck                     \
                            --leak-check=full                   \
                            --show-leak-kinds=all               \
                            --track-origins=yes                 \
                            ./build/compiler ${in}"
#                            --xtree-memory=full                 \
#                            --xtree-memory-file=xtmemory.kcg    \
    eval "$valgrind_cmd"
fi

exit 0
