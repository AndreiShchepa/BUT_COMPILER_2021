cmake_minimum_required(VERSION 3.0)
project(compiler)


###############################################################################
# includes
include(FetchContent)
include(GoogleTest)
include(CodeCoverage.cmake)


###############################################################################
# c preferences
set(CC gcc)
set(COVERED_FLAGS "-ftest-coverage -fprofile-arcs --coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -Wall -Wextra -Werror ${COVERED_FLAGS}")
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE ON)


###############################################################################
# cpp preferences
set(CXX g++)
set(CMAKE_CXX_STANDARD 11)                              # This property specifies the C++ standard whose features are requested to build this target.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)                   # required standart
set(CMAKE_CXX_EXTENSIONS OFF)                           # when ON(default) is used -std=gnu++?? instead of -std=c++??
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-g -Wall -Wextra -pedantic ${COVERED_FLAGS}")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)              # to get *.o files => for code covered
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)                   # for vim tags

###############################################################################
# sources
set(COMMON_SOURCES
    ./src/debug.h
    ./src/error.h
    ./src/str.c src/str.h
)

set(COMPILER_SOURCES
    src/compiler.c
    src/symtable.c src/symtable.h
    src/scanner.c src/scanner.h
    ${COMMON_SOURCES}
)

set(SCANNER_TESTS_SOURCES
    ./tests/scannerTests.cpp
    ./src/scanner.c ./src/scanner.h
    ${COMMON_SOURCES}
)

set(RETURN_CODES_TESTS_SOURCES
    ./tests/returnCodesTests.cpp
    ./src/scanner.c ./src/scanner.h
    ${COMMON_SOURCES}
)


###############################################################################
# project
add_executable(${PROJECT_NAME} ${COMPILER_SOURCES})
target_link_libraries(${PROJECT_NAME})


###############################################################################
# TESTS
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        )
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(scannerTests ${SCANNER_TESTS_SOURCES})
add_executable(returnCodesTests ${RETURN_CODES_TESTS_SOURCES})

target_link_libraries(scannerTests gtest_main)
target_link_libraries(returnCodesTests gtest_main)

gtest_discover_tests(scannerTests)
gtest_discover_tests(returnCodesTests)


#################################################################################
### TESTS COVERED
setup_target_for_coverage(scannerTests_coverage scannerTests coverage)
setup_target_for_coverage(returnCodeTests_coverage returnCodeTests coverage)
