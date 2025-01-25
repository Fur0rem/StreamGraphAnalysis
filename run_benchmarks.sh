#!/bin/bash

CC=gcc
CFLAGS="-O4"

SRC_DIR=src
BENCH_DIR=benchmarks
BIN_DIR=bin/benchmark

TEXT_BOLD=$(tput bold)
TEXT_RESET=$(tput sgr0)
TEXT_RED=$(tput setaf 1)
TEXT_GREEN=$(tput setaf 2)

global_success=0

make clean
make libSGA compile_mode=benchmark
# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "${TEXT_BOLD}${TEXT_RED}Compilation failed for libSGA${TEXT_RESET}"
    exit 1
fi

# Compile benchmark.c into an object file
$CC $CFLAGS -c $BENCH_DIR/benchmark.c -o $BIN_DIR/benchmark.o

# If you have only one argument : run that benchmark only
if [ $# -eq 1 ]; then
    filename=$1
    echo "Found benchmark file: $filename"

    # Compile the file with libSGA.a
    $CC $CFLAGS -o $BIN_DIR/benchmark_$filename $BENCH_DIR/$filename.c $BIN_DIR/libSGA.a $BIN_DIR/benchmark.o

    # Run the benchmark
    $BIN_DIR/benchmark_$filename
    # Check the return code
    if [ $? -ne 0 ]; then
        global_success=1
    fi

    echo ""
    if [ $global_success -eq 0 ]; then
        echo "All benchmarks passed!"
    else
        echo "${TEXT_BOLD}${TEXT_RED}TEST FAILED FOR $filename !!!${TEXT_RESET}"
    fi

    exit $global_success
fi

# Iterate over all files in the benchmarks directory
every_benchmark_that_failed=""
every_benchmark_which_failed_compilation=""
every_benchmark_that_passed=""
for file in $BENCH_DIR/*.c; do
    # If it is benchmark.c or a file that does not end in .c, skip it
    if [ $(basename $file) == "benchmark.c" ] || [ $(grep -q ".c" $file | wc -l) != 0 ]; then
        continue
    fi
    # Get the filename without the extension
    filename=$(basename $file .c)
    echo "Found benchmark file: $filename"

    # Compile the file with libSGA.a
    $CC $CFLAGS -o $BIN_DIR/benchmark_$filename $BENCH_DIR/$filename.c $BIN_DIR/libSGA.a $BIN_DIR/benchmark.o

    # Check if the compilation was successful
    if [ $? -ne 0 ]; then
        echo "${TEXT_BOLD}${TEXT_RED}Compilation failed for $filename${TEXT_RESET}"
        every_benchmark_which_failed_compilation="$every_benchmark_which_failed_compilation $filename"
        global_success=1
        continue
    fi

    # Run the benchmark
    $BIN_DIR/benchmark_$filename
    # Check the return code
    if [ $? -ne 0 ]; then
        every_benchmark_that_failed="$every_benchmark_that_failed $filename"
        global_success=1
    else
        every_benchmark_that_passed="$every_benchmark_that_passed $filename"
    fi

    echo ""

done

if [ $global_success -eq 0 ]; then
    echo "All benchmarks ran successfully!"
else
    echo "${TEXT_BOLD}${TEXT_RED} Some benchmarks failed!${TEXT_RESET}"
    echo "${TEXT_GREEN}  Benchmarks that ran: ${TEXT_RESET} $every_benchmark_that_passed"
    echo "${TEXT_BOLD} ${TEXT_RED} Benchmarks that failed: ${TEXT_RESET} $every_benchmark_that_failed"
    echo "${TEXT_BOLD} ${TEXT_RED} Benchmarks that failed compilation: ${TEXT_RESET} $every_benchmark_which_failed_compilation"
fi
exit $global_success
