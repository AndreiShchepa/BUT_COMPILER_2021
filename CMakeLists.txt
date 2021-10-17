cmake_minimum_required(VERSION 3.0)
project(compiler)


###############################################################################
# includes
include(FetchContent)


###############################################################################
# c preferences
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -g")


###############################################################################
# cpp preferences
set(CMAKE_CXX_STANDARD 17)                              # This property specifies the C++ standard whose features are requested to build this target.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)                   # required standart
set(CMAKE_CXX_EXTENSIONS OFF)                           # when ON(default) is used -std=gnu++?? instead of -std=c++??
set(POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g")


###############################################################################
# project
ADD_EXECUTABLE(compiler
    src/compiler.c
    src/str.c src/str.h
    src/symtable.c src/symtable.h
    src/debug.h
    src/scanner.c src/scanner.h
    src/error.h)


###############################################################################
# names of tests
set(scanner_t scannerTests)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        ${scanner_t}
        ./tests/scannerTests.cpp
        ./src/scanner.c
        ./src/str.c
        ./src/error.h
)

target_link_libraries( ${scanner_t} gtest_main )

include(GoogleTest)
gtest_discover_tests(${scanner_t})
