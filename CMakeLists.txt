cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -g")

option(DEBUG_RULES   "Enable rules printing from parser"       OFF)
option(DEBUG_SCANNER "Run only scanner"                        OFF)
option(DEBUG_EXPR    "Enable debug for expressions processing" OFF)

if (DEBUG_RULES)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_RULES")
endif()

if (DEBUG_SCANNER)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_SCANNER")
endif()

if (DEBUG_EXPR)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_EXPR")
endif()

project(compiler)

ADD_EXECUTABLE(compiler
    src/compiler.c
    src/str.c src/str.h
    src/symtable.c src/symtable.h
    src/debug.h
    src/scanner.c src/scanner.h
    src/error.h
    src/parser.h src/parser.c
    src/expressions.c src/expressions.h)
