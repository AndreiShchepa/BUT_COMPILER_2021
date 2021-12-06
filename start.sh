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
tags=0
run=0
[[ "$#" -eq 0 ]] && iszero=1 || iszero=0


error_exit() {
    echo "ERROR"
    exit 1
}

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
    "BUILT_IN")
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
    "--tags")
        tags=1
        ;;
    "--run")
        run=1
        ;;
    "--help")
        help=1
        ;;
    esac
    shift
done

# remove from tests_code_gen compiled codes
if [ "$clean" -eq 1 ]; then
    cd without_errors || error_exit
    rm *.lua
    rm *.ifjcode
    rm *.out
    cd .. || error_exit

    cd without_errors_input || error_exit
    rm *.lua
    rm *.ifjcode
    rm *.out
    cd .. || error_exit
fi

if [ "$help" -eq 1 ] || [ "$iszero" -eq 1 ]; then
    echo "USAGE:"
    echo "# file must have .tl extension"
    echo ""
    echo "./start                                       # does nothing"
    echo "./start --compile filename.tl --exec --out    # compile, execute and print to file_basename.ifjcode"
    echo "./start --compile filename.tl --exec          # compile, execute and print to stdout"
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

if [ "$run" -eq 1 ]; then
    cd "$(dirname "${in}")" || exit 1
    run_cmd="./ic21int $(basename "$in" .tl).ifjcode"
    eval "$run_cmd"
    cd .. || exit 1
fi

# compile to lua all files in tests_code_gen
if [ "$compile_to_lua" -eq 1 ]; then
    cd without_errors || exit 1
    for file in *.tl; do
        compile_cmd="tl gen \"$file\""
        eval "$compile_cmd" || exit 1
    done
    cd ..
    cd without_errors_input || exit 1
    for file in *.tl; do
        compile_cmd="tl gen \"$file\""
        eval "$compile_cmd" || exit 1
    done
    cd ..
fi

# compile to ifjcode all files in tests_code_gen
if [ "$compile_to_ifjcode" -eq 1 ]; then
    cd build || exit 1
    for file in ../without_errors/*.tl; do
#        echo $file
        if [ "$file" != "../without_errors/ifj21.tl" ]; then
            echo "${file}"
            compile_cmd="./compiler <${file} >${file%.*}.ifjcode"
            eval "$compile_cmd" || exit 1
        fi
    done
    for file in ../without_errors_input/*.tl; do
#        echo $file
        if [ "$file" != "../without_errors_input/ifj21.tl" ]; then
            echo "${file}"
            compile_cmd="./compiler <${file} >${file%.*}.ifjcode"
            eval "$compile_cmd" || exit 1
        fi
    done
    cd .. || exit 1
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

if [ "$tags" -eq 1 ]; then
    ctags_cmd="ctags -R ."
    cscope_cmd="cscope -Rb"
    eval "$ctags_cmd"
    eval "$cscope_cmd"
fi

exit 0
