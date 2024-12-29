#!/bin/bash

# Print current directory
echo "Current directory: $(pwd)"

# Print file existence checks
echo "Checking for required files..."
[ -f "main.c" ] && echo "main.c exists" || echo "main.c not found"
[ -f "../lowpass_filter_1storder/lowpass_filter_1storder.h" ] && echo "header file exists" || echo "header file not found"

# Compile the program with more verbose output
echo "Compiling lowpass filter test..."
gcc -v main.c -o lowpass_test -lm -O2 -Wall

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    
    # Run the program
    echo "Running test..."
    ./lowpass_test
    
    # Check if output file was created
    [ -f "lpf_results.txt" ] && echo "Results file created successfully" || echo "No results file generated"
    
    # Check if Python is installed for plotting
    if command -v python3 &>/dev/null; then
        echo "Creating plots..."
        python3 plot_results.py
    else
        echo "Python not found. Please install Python to generate plots."
    fi
else
    echo "Compilation failed!"
    exit 1
fi