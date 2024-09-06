#!/bin/bash

CC=gcc
CFLAGS="-Wall -Wextra -g -Wno-unused-function"

SRC_DIR=src
benchmark_DIR=benchmarks
BIN_DIR=bin

TEXT_BOLD=$(tput bold)
TEXT_RESET=$(tput sgr0)
TEXT_RED=$(tput setaf 1)

global_success=0

# Compile benchmark.c into an object file
$CC $CFLAGS -c $benchmark_DIR/benchmark.c -o $BIN_DIR/benchmark.o

# Check if the --valgrind flag is present
valgrind=0
if [ "$1" == "--valgrind" ]; then
    valgrind=1
    shift
fi

# If you have only one argument : run that benchmark only
if [ $# -eq 1 ]; then
    filename=$1
    echo "Found benchmark file: $filename"
    # Remove old benchmark file
    rm -f $BIN_DIR/benchmark_$filename
    rm -f $BIN_DIR/$filename.a
    rm -f $BIN_DIR/$filename.o
    # If the src file.c does not exist, compile the benchmark file into an executable (header only library)
    if [ ! -f $SRC_DIR/$filename.c ]; then
        $CC $CFLAGS -o $BIN_DIR/benchmark_$filename $benchmark_DIR/$filename.c $BIN_DIR/benchmark.o
    else
        make $filename
        if [ -f $BIN_DIR/$filename.a ]; then
            $CC $CFLAGS -o $BIN_DIR/benchmark_$filename $benchmark_DIR/$filename.c $BIN_DIR/$filename.a $BIN_DIR/benchmark.o
        else
            $CC $CFLAGS -o $BIN_DIR/benchmark_$filename $benchmark_DIR/$filename.c $BIN_DIR/$filename.o $BIN_DIR/benchmark.o
        fi
    fi

    if [ $valgrind -ne 1 ]; then
        $BIN_DIR/benchmark_$filename
    else
        valgrind --tool=callgrind $BIN_DIR/benchmark_$filename --callgrind-out-file=benchmarks/callgrind/$filename.out
    fi
    # Check the return code
    if [ $? -ne 0 ]; then
        global_success=1
    fi

    echo ""
    if [ $global_success -eq 0 ]; then
        echo "All benchmarks passed!"
    else
        echo "${TEXT_BOLD}${TEXT_RED}benchmark FAILED FOR $filename !!!${TEXT_RESET}"
    fi

    exit $global_success
fi

# Iterate over all files in the benchmarks directory
every_benchmark_that_failed=""
for file in $benchmark_DIR/*.c; do
    # If it is benchmark.c or a file that does not end in .c, skip it
    if [ $(basename $file) == "benchmark.c" ] || [ $(grep -q ".c" $file | wc -l) != 0 ]; then
        continue
    fi
    # Get the filename without the extension
    filename=$(basename $file .c)
    echo "Found benchmark file: $filename"
    # Remove old benchmark file
    rm -f $BIN_DIR/benchmark_$filename
    # If the src file.c does not exist, compile the benchmark file into an executable
    if [ ! -f $SRC_DIR/$filename.c ]; then
        $CC $CFLAGS -o $BIN_DIR/benchmark_$filename $file $BIN_DIR/benchmark.o
    else
        make $filename
        # If the compilation produced a .a file, use it instead of the .o file
        if [ -f $BIN_DIR/$filename.a ]; then
            $CC -Wno-unused-function -g -o $BIN_DIR/benchmark_$filename $file $BIN_DIR/$filename.a $BIN_DIR/benchmark.o
        else
            $CC -Wno-unused-function -g -o $BIN_DIR/benchmark_$filename $file $BIN_DIR/$filename.o $BIN_DIR/benchmark.o
        fi
    fi

    # If the file is present but not the benchmark, the compilation of the benchmark failed
    # If neither are present, the compilation of the file failed
    if [ ! -f $BIN_DIR/benchmark_$filename ] && [ -f $BIN_DIR/$filename.o ]; then
        echo "${TEXT_BOLD}${TEXT_RED}COMPILATION FAILED FOR benchmark OF $filename !!!${TEXT_RESET}"
        every_benchmark_that_failed="$every_benchmark_that_failed compilation_benchmark_$filename"
        global_success=1
        continue
    elif [ ! -f $BIN_DIR/benchmark_$filename ]; then
        echo "${TEXT_BOLD}${TEXT_RED}COMPILATION FAILED FOR $filename !!!${TEXT_RESET}"
        every_benchmark_that_failed="$every_benchmark_that_failed compilation_$filename"
        global_success=1
        continue
    fi


    # $BIN_DIR/benchmark_$filename
    valgrind --tool=callgrind $BIN_DIR/benchmark_$filename --callgrind-out-file=benchmarks/callgrind/$filename.out

    # Check the return code
    if [ $? -ne 0 ]; then
        every_benchmark_that_failed="$every_benchmark_that_failed $filename"
        global_success=1
    fi

    echo ""

done

if [ $global_success -eq 0 ]; then
    echo "All benchmarks passed!"
else 
    echo "${TEXT_BOLD} ${TEXT_RED} benchmarks that failed: $every_benchmark_that_failed ${TEXT_RESET}"
fi
exit $global_success