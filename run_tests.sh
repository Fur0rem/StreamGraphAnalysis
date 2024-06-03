#!/bin/bash

CC=gcc
CFLAGS=-Wall

SRC_DIR=src
TEST_DIR=tests
BIN_DIR=bin

global_success=0

# Compile test.c into an object file
$CC $CFLAGS -c $TEST_DIR/test.c -o $BIN_DIR/test.o

# Iterate over all files in the tests directory
for file in $TEST_DIR/*.c; do
    # If it is test.c or test.h, skip it
    if [ $(basename $file) == "test.c" ] || [ $(basename $file) == "test.h" ]; then
        continue
    fi
    # Get the filename without the extension
    filename=$(basename $file .c)
    echo "Found test file: $filename"
    # Compile the src file
    make $filename
    # Compile the test file into an executable
    $CC $CFLAGS -o $BIN_DIR/$filename $file $filename.o $BIN_DIR/test.o

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