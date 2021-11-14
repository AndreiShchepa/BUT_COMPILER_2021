#!/bin/bash

RED='\033[0;31m'
NC='\033[0m'
GREEN='\033[0;32m'

cd build/ && rm -rf * && cmake .. && make && cd ..

echo ""

for file in tests_semantic/*_3.tl;
do
    valgrind --log-file="tmp.txt" build/compiler <$file 2>/dev/null
    ret_val=$?
    if [[ "$ret_val" == "3" ]]; then
        printf "$file ${GREEN}PASSED${NC} ret_val = $ret_val\n"
    else
        printf "$file ${RED}FAILED${NC} ret_val = $ret_val\n"
    fi

    OUT1=$(cat tmp.txt | grep -h 'in use at exit:')
    OUT2=$(cat tmp.txt | grep -h 'errors from')

    if [[ "$OUT1" == *"0 bytes in 0 blocks"* ]]; then
        if [[ "$OUT2" != *"0 errors from 0 contexts"* ]]; then
            printf "$file ${RED}ERROR${NC} with memory\n"
        fi
    else
        printf "$file ${RED}ERROR${NC} with memory\n"
    fi
    echo ""
done

rm tmp.txt
