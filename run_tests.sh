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
# Check if the -valgrind flag is set
if [ "$1" == "-valgrind" ]; then
    shift
    valgrind=1
else 
    valgrind=0
fi

if [ $# -eq 1 ]; then
    filename=$1
    echo "Found test file: $filename"
    # If the src file.c does not exist, compile the test file into an executable (header only library)
    if [ ! -f $SRC_DIR/$filename.c ]; then
        $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/test.o
    else
        make $filename
        if [ -f $BIN_DIR/$filename.a ]; then
            $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/$filename.a $BIN_DIR/test.o
        else
            $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $filename.o $BIN_DIR/test.o
        fi
    fi

    if [ $valgrind -ne 1 ]; then
        $BIN_DIR/test_$filename
    else
        valgrind -s $BIN_DIR/test_$filename --track-origin
    fi
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
        $CC $CFLAGS -o $BIN_DIR/test_$filename $file $BIN_DIR/test.o
    else
        make $filename
        # If the compilation produced a .a file, use it instead of the .o file
        if [ -f $BIN_DIR/$filename.a ]; then
            $CC -Wno-unused-function -g -o $BIN_DIR/test_$filename $file $BIN_DIR/$filename.a $BIN_DIR/test.o
        else
            $CC -Wno-unused-function -g -o $BIN_DIR/test_$filename $file $BIN_DIR/$filename.o $BIN_DIR/test.o
        fi
    fi

    $BIN_DIR/test_$filename
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