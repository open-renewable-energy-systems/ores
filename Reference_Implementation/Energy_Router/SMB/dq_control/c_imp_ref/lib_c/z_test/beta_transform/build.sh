#!/bin/bash

# Directory paths
LIB_DIR="../../"
TEST_DIR="."

# Create build directory if it doesn't exist
mkdir -p build

# First, verify all source files exist
if [ ! -f "${TEST_DIR}/main.c" ]; then
    echo "Error: main.c not found!"
    exit 1
fi

if [ ! -f "${LIB_DIR}beta_transform/beta_transform_1p.c" ]; then
    echo "Error: beta_transform_1p.c not found!"
    exit 1
fi

# Compile command
gcc -o build/test_beta_transform \
    ${TEST_DIR}/main.c \
    ${LIB_DIR}beta_transform/beta_transform_1p.c \
    -I${LIB_DIR}beta_transform \
    -I${LIB_DIR}lowpass_filter \
    -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Executable created at build/test_beta_transform"
    # Run the test
    ./build/test_beta_transform
else
    echo "Build failed!"
    exit 1
fi