#!/bin/bash

CC=gcc
CFLAGS="-Wall -Wextra -g"

SRC_DIR=src
TEST_DIR=tests
BIN_DIR=bin

global_success=0

# Compile test.c into an object file
$CC $CFLAGS -c $TEST_DIR/test.c -o $BIN_DIR/test.o

# If you have only one argument : run that test only
if [ $# -eq 1 ]; then
    filename=$1
    echo "Found test file: $filename"
    # If the src file.c does not exist, compile the test file into an executable
    if [ ! -f $SRC_DIR/$filename.c ]; then
        $CC $CFLAGS -o $BIN_DIR/$filename $TEST_DIR/$filename.c $BIN_DIR/test.o
    else
        # Compile the src file
        make $filename
        # Compile the test file into an executable
        $CC $CFLAGS -o $BIN_DIR/$filename $TEST_DIR/$filename.c $filename.o $BIN_DIR/test.o
    fi

    # Run the test
    valgrind -s $BIN_DIR/$filename --track-origin
    # Check the return code
    if [ $? -ne 0 ]; then
        global_success=1
    fi

    echo ""
    if [ $global_success -eq 0 ]; then
        echo "All tests passed!"
    fi
    exit $global_success
fi

# Iterate over all files in the tests directory
for file in $TEST_DIR/*.c; do
    # If it is test.c or test.h, skip it
    if [ $(basename $file) == "test.c" ] || [ $(basename $file) == "test.h" ]; then
        continue
    fi
    # Get the filename without the extension
    filename=$(basename $file .c)
    echo "Found test file: $filename"
    # If the src file.c does not exist, compile the test file into an executable
    if [ ! -f $SRC_DIR/$filename.c ]; then
        $CC $CFLAGS -o $BIN_DIR/$filename $file $BIN_DIR/test.o
    else
        # Compile the src file
        make $filename
        # Compile the test file into an executable
        $CC $CFLAGS -o $BIN_DIR/$filename $file $filename.o $BIN_DIR/test.o
    fi

    # Run the test
    $BIN_DIR/$filename
    # Check the return code
    if [ $? -ne 0 ]; then
        global_success=1
    fi

    echo ""

done

if [ $global_success -eq 0 ]; then
    echo "All tests passed!"
fi
exit $global_success