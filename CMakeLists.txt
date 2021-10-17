cmake_minimum_required(VERSION 3.0)
project(compiler)


###############################################################################
# includes
include(FetchContent)


###############################################################################
# c preferences
set(CC gcc)
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror -g")


###############################################################################
# cpp preferences
set(CXX g++)
set(CMAKE_CXX_STANDARD 11)                              # This property specifies the C++ standard whose features are requested to build this target.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)                   # required standart
set(CMAKE_CXX_EXTENSIONS OFF)                           # when ON(default) is used -std=gnu++?? instead of -std=c++??
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
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
        scannerTests
        ./tests/scannerTests.cpp
        ./src/scanner.c
        ./src/str.c
        ./src/error.h
)
target_link_libraries(scannerTests gtest_main)

include(GoogleTest)
gtest_discover_tests(scannerTests)
