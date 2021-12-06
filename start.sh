#!/bin/bash
complete -c timedatectl -s h -l help -d 'Print a short help text and exit'

set_dbg_cmake() {
    params="$params -DDEBUG_$1=ON"
}

set_compile_opts() {
    params="$params -DCOMPILE_$1=on"
}

usage() {
    echo "USAGE: all switchers"
    echo "params=""                   # params for cmake"
    echo "in=""                       # input_filename take last .tl file"
    echo ""
    echo "run_compiler=0              # build || build and execute"
    echo "run_lua=0                   # run lua code"
    echo "run_ifjcode=0               # run ifjcode"
    echo ""
    echo "stdout=0                    # if 1 to stdout else to in.ifjcode"
    echo ""
    echo "build=0                     # build project (compiler)"
    echo "build_lua=0                 # build lua"
    echo "build_ifjcode=0             # build ifjcode"
    echo ""
    echo "valgrind=0                  # run valgrind with input file"
    echo "clean=0                     # clean without_error dir and all dirs inside it  =>  rm {*.lua, *.ifjcode, *.out}"
    echo ""
    echo "tags=0"
    exit 0
}

error_exit() {
    echo "ERROR"
    exit 1
}


#############
#############
params=""                   # params for cmake
in=""                       # input_filename take last .tl file

run_compiler=0              # build || build and execute
run_lua=0                   # run lua code
run_ifjcode=0               # run ifjcode

stdout=0                    # if 1 to stdout else to in.ifjcode

build=0                     # build project (compiler)
build_lua=0                 # build lua
build_ifjcode=0             # build ifjcode

valgrind=0                  # run valgrind with input file
clean=0                     # clean without_error dir and all dirs inside it  =>  rm {*.lua, *.ifjcode, *.out}

tags=0

[[ "$#" -eq 0 ]] && usage


#################
#   SWITCHERS
#################
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
    "--clean")
        clean=1
        ;;
    *".tl")
        in="$1"
        ;;
    "--stdout")
        stdout=1
        ;;
    "--build")
        build=1
        ;;
    "--build_lua")
        build_lua=1
        ;;
    "--build_ifjcode")
        build_ifjcode=1
        ;;
    "--run_compiler")
        run_compiler=1
        ;;
    "--run_ifjcode")
        run_ifjcode=1
        ;;
    "--valgrind")
        valgrind=1
        ;;
    "--tags")
        tags=1
        ;;
    "--help")
        usage
        ;;
    esac
    shift
done


#######################
#       CLEAN
#######################
if [ "$clean" -eq 1 ]; then
    cd without_errors || error_exit
    for name in *; do
        if [ -d "${name}" ]; then
            cd "${name}" || error_exit
            rm *.ifjcode
            rm *.lua
            rm *.out
            cd .. || error_exit
        fi
        if [ -f "${name}" ]; then
            rm *.ifjcode
            rm *.lua
            rm *.out
        fi
    done
    cd .. || error_exit
fi


if [ ! -d "build/" ]; then
    mkdir build
fi



#######################
#       BUILD
#######################
if [ "$build" -eq 1 ]; then          # build project or  build prokect and "$in" file
    cd build || error_exit
    eval "rm -rf *"
    eval "cmake $params .."
    eval "make -j16"
    cd .. || error_exit
fi


declare -a without_errors_folders=("input" \
                                   "buitin_func" \
                                   "nil" \
                                   "write_value"  \
                                   "zero")
if [ "$build_lua" -eq 1 ]; then
    echo "TODO"
#    cd without_errors || error_exit
#    for i in "${without_errors_folders[@]}"; do
#        for name in without_errors/${i}/*.tl; do
#            echo  $(dirname ${name})
#            cd $(dirname ${name}) || error_exit
#            f=$(basename ${name})
#            echo  ${f}
#            if [[ -f "${f}" ]] ; then
#                echo ${f}
##                compile_cmd="tl gen \"${name}\""
##                eval "$compile_cmd" || error_exit
#            fi
#            cd ../.. || error_exit
#        done
#    done
#    cd .. || error_exit
fi


if [ "$build_ifjcode" -eq 1 ]; then
    cd build || exit 1
    for file in ../without_errors/*.tl; do
#        echo $file
        if [ "$file" != "../without_errors/ifj21.tl" ]; then
            echo "${file}"
            compile_cmd="./compiler <${file} >${file%.*}.ifjcode"
            eval "$compile_cmd" || exit 1
        fi
    done
    cd .. || exit 1
fi


#######################
#       RUN
#######################
if [ "$run_compiler" -eq 1 ] && [ "$in" != "" ];then
    if [ "$stdout" -eq 0 ]; then          # --run_compiler && --out => create compiled .ifjcode
        eval "./build/compiler <$in >${in%.*}.ifjcode"
    else                             # --exec => build and print to stdout
        eval "./build/compiler <$in"
    fi
fi


if [ "$run_ifjcode" -eq 1 ]; then
    cd "$(dirname "${in}")" || exit 1
    run_cmd="./ic21int $(basename "$in" .tl).ifjcode"
    eval "$run_cmd"
    cd .. || exit 1
fi


if [ "$valgrind" -eq 1 ]; then
    valgrind_cmd="valgrind  --tool=memcheck                     \
                            --leak-check=full                   \
                            --show-leak-kinds=all               \
                            --track-origins=yes                 \
                            --log-file="valgrind.out"           \
                            ./build/compiler < ${in}"
    eval "$valgrind_cmd"
fi


#######################
#       OTHERS
#######################
if [ "$tags" -eq 1 ]; then
    ctags_cmd="ctags -R ."
    cscope_cmd="cscope -Rb"
    eval "$ctags_cmd"
    eval "$cscope_cmd"
fi

exit 0
