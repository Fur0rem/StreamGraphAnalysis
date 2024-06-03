#!/bin/bash

CC=gcc
CFLAGS=-Wall

# Change directory to the tests directory
cd tests/

# Iterate over all files in the directory
for file in *; do
    # Check if the file is a regular file
    if [[ -f $file ]]; then
        # Compile the src file
        make "$file"

        # Compile the test file using the .o file generated
        #$CC $CFLAGS -o "$file" "$file".o ../src/*.o

        # Run the test
        ./"$file"
    fi
done