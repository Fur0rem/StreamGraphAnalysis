#!/bin/bash

CC=gcc

SRC_DIR=src
PROF_DIR=profiling
BIN_DIR=bin/profiling

TEXT_BOLD=$(tput bold)
TEXT_RESET=$(tput sgr0)
TEXT_RED=$(tput setaf 1)
TEXT_GREEN=$(tput setaf 2)

global_success=0

make clean
make libSGA compile_mode=profiling
make profiling compile_mode=profiling -B

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "${TEXT_BOLD}${TEXT_RED}Compilation failed for libSGA${TEXT_RESET}"
    exit 1
fi

# If you have only one argument : run that profiling only
if [ $# -eq 1 ]; then
    filename=$1
    echo "Found profiling file: $filename"

    # Run the profiling
    echo "Going to run profiling_$filename"
    perf record -F 99 -g --call-graph dwarf -- $BIN_DIR/profiling_$filename
    perf script > out.perf
    perf script | stackcollapse-perf.pl > out.folded
    flamegraph.pl out.folded > flamegraph.svg
    mv flamegraph.svg $BIN_DIR/
    mv out.perf $BIN_DIR/
    mv out.folded $BIN_DIR/

    # Check the return code
    if [ $? -ne 0 ]; then
        global_success=1
    fi

    echo ""
    if [ $global_success -eq 0 ]; then
        echo "All profilings passed!"
    else
        echo "${TEXT_BOLD}${TEXT_RED}profiling FAILED FOR $filename !!!${TEXT_RESET}"
    fi



    exit $global_success
fi