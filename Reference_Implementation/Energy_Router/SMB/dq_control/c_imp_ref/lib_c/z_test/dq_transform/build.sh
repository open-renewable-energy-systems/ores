#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Clean everything first
rm -rf build/*

# Compile with assembly output preserved
echo "=== Compiling main program ==="
gcc -o build/dq_test \
    main.c \
    ../../dq_transform/dq_transform_1phase.c \
    -I../../ \
    -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "=== Running dq_test ==="
    ./build/dq_test
    echo "=== Now run: python3 plot_results.py ==="
else
    echo "Build failed!"
    exit 1
fi