cmake_minimum_required(VERSION 3.0)
project(compiler)


###############################################################################
# includes
include(FetchContent)
include(GoogleTest)


###############################################################################
# c preferences
set(CC gcc)
set(COVERAGE_FLAGS "-ftest-coverage -fprofile-arcs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -Wall -Wextra -Werror ${COVERAGE_FLAGS}")
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE ON)


###############################################################################
# cpp preferences
set(CXX g++)
set(CMAKE_CXX_STANDARD 11)                              # This property specifies the C++ standard whose features are requested to build this target.
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)                   # required standart
set(CMAKE_CXX_EXTENSIONS OFF)                           # when ON(default) is used -std=gnu++?? instead of -std=c++??
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-g -Wall -Wextra -pedantic ${COVERAGE_FLAGS}")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)              # to get *.o files => for code covered


###############################################################################
# other pref
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)                   # for vim tags
set(CMAKE_BUILD_TYPE Debug)


###############################################################################
# opts
option(DEBUG_RULES    "Enable rules printing from parser"       OFF)
option(DEBUG_SCANNER  "Run only scanner"                        OFF)
option(DEBUG_EXPR     "Enable debug for expressions processing" OFF)
option(DEBUG_INSTR    "Print formated ifjcode21"                OFF)
option(DEBUG_BUILT_IN "Compile tests and code_coverage"		    OFF)
option(COMPILE_TESTS  "Compile tests and code_coverage"		    OFF)

if (DEBUG_RULES)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_RULES")
endif()

if (DEBUG_SCANNER)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_SCANNER")
endif()

if (DEBUG_EXPR)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_EXPR")
endif()

if (DEBUG_INSTR)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_INSTR")
endif()

if (DEBUG_BUILD_IN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG_BUILD_IN")
endif()


###############################################################################
# sources
set(COMMON_SOURCES
	./src/debug.h
	./src/error.h
	./src/str.c src/str.h
	./src/queue.c src/queue.h
	)

set(COMPILER_SOURCES
	src/compiler.c
	src/symtable.c    src/symtable.h
	src/scanner.c     src/scanner.h
	src/parser.c      src/parser.h
	src/expressions.c src/expressions.h
	src/code_generator.c src/code_generator.h
    src/symstack.c    src/symstack.h
	${COMMON_SOURCES}
	)

set(SCANNER_TESTS_SOURCES
	./scanner_tests/scannerTests.cpp
	./src/scanner.c ./src/scanner.h
	${COMMON_SOURCES}
	)

set(RETURN_CODES_TESTS_SOURCES
	./scanner_tests/returnCodesTests.cpp
	./src/scanner.c ./src/scanner.h
	${COMMON_SOURCES}
	)


###############################################################################
# project
add_executable(${PROJECT_NAME} ${COMPILER_SOURCES})
target_link_libraries(${PROJECT_NAME})


###############################################################################
# TESTS
if (COMPILE_TESTS)
	include(CodeCoverage.cmake)
	FetchContent_Declare(googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG main
		)
	FetchContent_MakeAvailable(googletest)
	enable_testing()

	add_executable(scannerTests ${SCANNER_TESTS_SOURCES})
	add_executable(returnCodesTests ${RETURN_CODES_TESTS_SOURCES})

	target_link_libraries(scannerTests gtest_main)
	target_link_libraries(returnCodesTests gtest_main)

	gtest_discover_tests(scannerTests)
	gtest_discover_tests(returnCodesTests)

	### TESTS COVERAGE
	setup_target_for_coverage(scannerTests_coverage scannerTests scannerTests_coverage)
	setup_target_for_coverage(returnCodesTests_coverage returnCodesTests returnCodesTests_coverage)
endif()
