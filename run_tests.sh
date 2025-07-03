#!/bin/bash

CC=gcc
CFLAGS="-Wall -Wextra -g -Wno-unused-function -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=bounds -fno-sanitize-recover=all -Iinclude -Itests -IlibSGA/include"

SRC_DIR=src
TEST_DIR=tests
BIN_DIR=bin/debug

TEXT_BOLD=$(tput bold)
TEXT_RESET=$(tput sgr0)
TEXT_RED=$(tput setaf 1)
TEXT_GREEN=$(tput setaf 2)

global_success=0

make clean
make libSGA compile_mode=debug
# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "${TEXT_BOLD}${TEXT_RED}Compilation failed for libSGA${TEXT_RESET}"
    exit 1
fi

# Compile test.c into an object file
$CC $CFLAGS -c $TEST_DIR/test.c -o $BIN_DIR/test.o

# If you have only one argument : run that test only
if [ $# -eq 1 ]; then
    filename=$1
    echo "Found test file: $filename"

    # Compile the file with libSGA.a
    $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/libSGA.a $BIN_DIR/test.o

    # Run the test
    $BIN_DIR/test_$filename
    # Check the return code
    if [ $? -ne 0 ]; then
        global_success=1
    fi

    echo ""
    if [ $global_success -eq 0 ]; then
        echo "All tests passed!"
    else
        echo "${TEXT_BOLD}${TEXT_RED}TEST FAILED FOR $filename !!!${TEXT_RESET}"
    fi

    exit $global_success
fi

# Iterate over all files in the tests directory
every_test_that_failed=""
every_test_which_failed_compilation=""
every_test_that_passed=""
for file in $TEST_DIR/*.c; do
    # If it is test.c or a file that does not end in .c, skip it
    if [ $(basename $file) == "test.c" ] || [ $(grep -q ".c" $file | wc -l) != 0 ]; then
        continue
    fi
    # Get the filename without the extension
    filename=$(basename $file .c)
    echo "Found test file: $filename"

    # Compile the file with libSGA.a
    $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/libSGA.a $BIN_DIR/test.o

    # Check if the compilation was successful
    if [ $? -ne 0 ]; then
        echo "${TEXT_BOLD}${TEXT_RED}Compilation failed for $filename${TEXT_RESET}"
        every_test_which_failed_compilation="$every_test_which_failed_compilation $filename"
        global_success=1
        continue
    fi

    # Run the test
    $BIN_DIR/test_$filename
    echo "Running test for $BIN_DIR/test_$filename"
    # Check the return code
    if [ $? -ne 0 ]; then
        every_test_that_failed="$every_test_that_failed $filename"
        global_success=1
    else
        every_test_that_passed="$every_test_that_passed $filename"
    fi

    echo ""

done

if [ $global_success -eq 0 ]; then
    echo "All tests passed!"
else
    echo "${TEXT_BOLD}${TEXT_RED} Some tests failed!${TEXT_RESET}"
    echo "${TEXT_GREEN}  Tests that passed: ${TEXT_RESET} $every_test_that_passed"
    echo "${TEXT_BOLD} ${TEXT_RED} Tests that failed: ${TEXT_RESET} $every_test_that_failed"
    echo "${TEXT_BOLD} ${TEXT_RED} Tests that failed compilation: ${TEXT_RESET} $every_test_which_failed_compilation"
fi
exit $global_success
