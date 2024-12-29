#!/bin/bash

# Compiler settings
CC=gcc
CFLAGS="-Wall -Wextra -g"

# Source files
SOURCES="
    main.c
    ../../phase_lock/pll.c
    ../../phase_lock/pll_phase_detector/pll_phase_detector.c
    ../../phase_lock/pll_controller_pi/pll_controller_pi.c
    ../../phase_lock/pll_voltage_oscillator/vco_controller.c
    ../../log_data_rw/log_data_rw.c
    ../../notch_filter/notch_filter.c
"

# Output executable name
OUTPUT="pll_test"

# Compile command
echo "Compiling..."
$CC $CFLAGS -o $OUTPUT $SOURCES -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Executable: $OUTPUT"
    
    # Run the program
    echo "Running PLL test..."
    ./$OUTPUT
    
    # Check if program ran successfully
    if [ $? -eq 0 ]; then
        echo "PLL test completed successfully"
        
        # Run the Python plotting script
        echo "Generating plots..."
        python3 plot_results.py
        
        if [ $? -eq 0 ]; then
            echo "Plots generated successfully"
        else
            echo "Error generating plots"
            exit 1
        fi
    else
        echo "PLL test failed"
        exit 1
    fi
else
    echo "Compilation failed!"
    exit 1
fi
