#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'
GREEN='\033[0;32m'

cd build/ && rm -rf * && cmake .. && make && cd ..

name=$1
expected_err=$2
valgrind_en=$3
folder=""
err_files=0
err_memory=0
all_files=0

echo ""
if [[ "$expected_err" -eq "0" ]]; then
    folder="without_errors"
elif [[ "$expected_err" -eq "2" ]]; then
    folder="syntax_errors"
elif [[ "$expected_err" -ge "3" ]] && [[ "$expected_err" -le "7" ]]; then
    folder="semantic_errors"
else
    echo "This script doesnt support this type of error $expected_error"
    exit 1
fi

for file in ${folder}/${name}*_${expected_err}.tl;
do
    if [[ "${folder}/${name}*_${expected_err}.tl" == "$file" ]]; then
        echo "Files with this arguments were not found"
        exit 1
    fi

    all_files=$((all_files+1))

    if [[ "$valgrind_en" == "valgrind" ]]; then
        valgrind --log-file="tmp.txt" build/compiler <$file 2>/dev/null
        ret_val=$?
        OUT1=$(cat tmp.txt | grep -h 'in use at exit:')
        OUT2=$(cat tmp.txt | grep -h 'errors from')
        if [[ "$OUT1" == *"0 bytes in 0 blocks"* ]]; then
            if [[ "$OUT2" != *"0 errors from 0 contexts"* ]]; then
                err_memory=$((err_memory+1))
                printf "$file ${RED}ERROR${NC} with memory\n"
                echo ""
            fi
        else
            err_memory=$((err_memory+1))
            printf "$file ${RED}ERROR${NC} with memory\n"
            echo ""
        fi

        rm tmp.txt
    else
        build/compiler <$file 2>/dev/null
        ret_val=$?
    fi

    if [ $ret_val -ne $expected_err ]; then
        err_files=$((err_files+1))
        printf "$file ${RED}FAILED${NC} ret_val = $ret_val, expected $expected_err\n"
        echo ""
    fi
done

if [[ "$err_files" -eq "0" ]] && [[ "$err_memory" -eq "0" ]] && [[ "$valgrind_en" == "valgrind" ]]; then
    printf "$all_files/$all_files tests were ${GREEN}PASSED${NC}\n"
    printf "No errors with memory\n"
elif [[ "$err_files" -eq "0" ]] && [[ "$valgrind_en" == "" ]]; then
    printf "$all_files/$all_files tests were ${GREEN}PASSED${NC}\n"
    printf "Errors with memory were not enabled\n"
else
    if [ $err_files -ne 0 ]; then
        printf "$err_files/$all_files tests were ${RED}FAILED${NC}\n"
        err_files=$((all_files-err_files))
        printf "$err_files/$all_files tests were ${GREEN}PASSED${NC}\n"
    else
        printf "$all_files/$all_files tests were ${GREEN}PASSED${NC}\n"
    fi

    if [[ "$valgrind_en" == "valgrind" ]] && [ $err_memory -ne 0 ]; then
        printf "$err_memory errors with memory was detected\n"
    elif [[ "$valgrind_en" == "valgrind" ]] && [ $err_memory -eq 0 ]; then
        printf "No errors with memory\n"
    else
        printf "Errors with memory were not enabled\n"
    fi
fi
