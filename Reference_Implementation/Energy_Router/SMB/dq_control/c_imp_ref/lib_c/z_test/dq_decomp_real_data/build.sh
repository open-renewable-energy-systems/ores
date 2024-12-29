#!/bin/bash

# Get the current directory
CURRENT_DIR=$(pwd)
echo "Current directory: $CURRENT_DIR"

# Create obj directory if it doesn't exist
mkdir -p obj

echo "Building program..."

# Compile source files
gcc -c -o obj/main.o main.c -I../../
gcc -c -o obj/beta_transform_1p.o ../../beta_transform/beta_transform_1p.c -I../../
gcc -c -o obj/dq_transform_1phase.o ../../dq_transform/dq_transform_1phase.c -I../../
gcc -c -o obj/dq_to_modulation.o ../../dq_to_modulation/dq_to_modulation.c -I../../
gcc -c -o obj/log_data_rw.o ../../log_data_rw/log_data_rw.c -I../../
gcc -c -o obj/notch_filter.o ../../notch_filter/notch_filter.c -I../../

# Link object files
gcc -o dq_decomp_real_data obj/main.o obj/beta_transform_1p.o obj/dq_transform_1phase.o \
    obj/dq_to_modulation.o obj/log_data_rw.o obj/notch_filter.o -lm

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    
    # Run the C program
    echo "Running dq_decomp_real_data..."
    ./dq_decomp_real_data
    
    # Check if the C program ran successfully
    if [ $? -eq 0 ]; then
        echo "C program execution successful!"
        
        # Run the Python plotting script
        echo "Generating plots..."
        python3 plot_results.py
        
        if [ $? -eq 0 ]; then
            echo "Plots generated successfully!"
        else
            echo "Error generating plots!"
            exit 1
        fi
    else
        echo "Error running C program!"
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi