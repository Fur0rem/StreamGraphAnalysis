#!/bin/bash

CC=gcc
CFLAGS="-Wall -Wextra -g -Wno-unused-function"

SRC_DIR=src
TEST_DIR=tests
BIN_DIR=bin

TEXT_BOLD=$(tput bold)
TEXT_RESET=$(tput sgr0)
TEXT_RED=$(tput setaf 1)

global_success=0

# Compile test.c into an object file
make clean
$CC $CFLAGS -c $TEST_DIR/test.c -o $BIN_DIR/test.o

# Check if the --valgrind flag is present
valgrind=0
if [ "$1" == "--valgrind" ]; then
    valgrind=1
    shift
fi

# If you have only one argument : run that test only
if [ $# -eq 1 ]; then
    filename=$1
    echo "Found test file: $filename"
    # Remove old test file
    rm -f $BIN_DIR/test_$filename
    rm -f $BIN_DIR/$filename.a
    rm -f $BIN_DIR/$filename.o
    # If the src file.c does not exist, compile the test file into an executable (header only library)
    # if [ ! -f $SRC_DIR/$filename.c ]; then
    #     $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/test.o
    # else
    #     make $filename
    #     if [ -f $BIN_DIR/$filename.a ]; then
    #         $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/$filename.a $BIN_DIR/test.o
    #     else
    #         $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/$filename.o $BIN_DIR/test.o
    #     fi
    # fi

    # Try to make the file
    result=$(make $filename 2>&1)
    # If it failed, it's a header only library
    if [ $? -ne 0 ]; then
        $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/test.o
    else
        # If the compilation produced a .a file, use it instead of the .o file
        if [ -f $BIN_DIR/$filename.a ]; then
            $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/$filename.a $BIN_DIR/test.o
        else
            $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/$filename.o $BIN_DIR/test.o
        fi
    fi
    
    if [ $valgrind -ne 1 ]; then
        $BIN_DIR/test_$filename
    else
        valgrind --track-origins=yes --leak-check=full -s $BIN_DIR/test_$filename
    fi
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
for file in $TEST_DIR/*.c; do
    # If it is test.c or a file that does not end in .c, skip it
    if [ $(basename $file) == "test.c" ] || [ $(grep -q ".c" $file | wc -l) != 0 ]; then
        continue
    fi
    # Get the filename without the extension
    filename=$(basename $file .c)
    echo "Found test file: $filename"
    # Remove old test file
    rm -f $BIN_DIR/test_$filename
    # Try to make the file
    result=$(make $filename 2>&1)
    # If it failed, it's a header only library
    if [ $? -ne 0 ]; then
        $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/test.o
    else
        # If the compilation produced a .a file, use it instead of the .o file
        if [ -f $BIN_DIR/$filename.a ]; then
            $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/$filename.a $BIN_DIR/test.o
        else
            $CC $CFLAGS -o $BIN_DIR/test_$filename $TEST_DIR/$filename.c $BIN_DIR/$filename.o $BIN_DIR/test.o
        fi
    fi

    # If the file is present but not the test, the compilation of the test failed
    # If neither are present, the compilation of the file failed
    if [ ! -f $BIN_DIR/test_$filename ] && [ -f $BIN_DIR/$filename.o ]; then
        echo "${TEXT_BOLD}${TEXT_RED}COMPILATION FAILED FOR TEST OF $filename !!!${TEXT_RESET}"
        every_test_that_failed="$every_test_that_failed compilation_test_$filename"
        global_success=1
        continue
    elif [ ! -f $BIN_DIR/test_$filename ]; then
        echo "${TEXT_BOLD}${TEXT_RED}COMPILATION FAILED FOR $filename !!!${TEXT_RESET}"
        every_test_that_failed="$every_test_that_failed compilation_$filename"
        global_success=1
        continue
    fi


    $BIN_DIR/test_$filename
    # Check the return code
    if [ $? -ne 0 ]; then
        every_test_that_failed="$every_test_that_failed $filename"
        global_success=1
    fi

    echo ""

done

if [ $global_success -eq 0 ]; then
    echo "All tests passed!"
else 
    echo "${TEXT_BOLD} ${TEXT_RED} Tests that failed: $every_test_that_failed ${TEXT_RESET}"
fi
exit $global_success