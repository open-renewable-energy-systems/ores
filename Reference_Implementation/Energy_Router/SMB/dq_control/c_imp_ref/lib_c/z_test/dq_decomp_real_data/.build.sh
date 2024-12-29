#!/bin/bash

echo "Current directory: $(pwd)"
echo "Building program..."

# Remove any existing copies
rm -f dq_to_modulation.*

# Copy required files locally and verify
cp ../../dq_to_modulation/dq_to_modulation.* .
echo "=== Checking copied files ==="
ls -l dq_to_modulation.*

echo "=== Content of dq_to_modulation.h ==="
cat dq_to_modulation.h

echo "=== Content of dq_to_modulation.c ==="
cat dq_to_modulation.c

echo "=== Compiling main program ==="

# First compile to assembly to check the function call
echo "=== Checking assembly of main.c ==="
gcc -S main.c -I. -I../../
cat main.s | grep modulation

# Now compile everything
gcc -v -Wall -I. -I../../ -D_USE_MATH_DEFINES \
    dq_to_modulation.c \
    ../../beta_transform/beta_transform_1p.c \
    ../../dq_transform/dq_transform_1phase.c \
    main.c \
    -o dq_decomp_real_data \
    -lm

if [ $? -eq 0 ]; then
    echo "Build successful!"
else
    echo "Build failed!"
    echo "=== Checking symbols in object files ==="
    gcc -c dq_to_modulation.c
    nm dq_to_modulation.o | grep modulation
    exit 1
fi