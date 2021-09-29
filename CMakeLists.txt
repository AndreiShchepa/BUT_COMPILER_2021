cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -g")

project(compiler)

ADD_EXECUTABLE(compiler
    src/compiler.c
    src/str.c src/str.h
    src/symtable.c src/symtable.h
    src/debug.h)
