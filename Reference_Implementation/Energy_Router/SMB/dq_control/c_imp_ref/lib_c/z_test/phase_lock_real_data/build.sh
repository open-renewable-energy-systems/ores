#!/bin/bash

# Set compiler flags
CFLAGS="-Wall -Wextra"
LIBS="-lm"

# Clean previous builds
rm -f pll_test pll_test_results.txt pll_test_plots.png

# Compile
echo "Compiling..."
gcc $CFLAGS -o pll_test main.c \
    ../../phase_lock/pll.c \
    ../../phase_lock/pll_phase_detector/pll_phase_detector.c \
    ../../phase_lock/pll_controller_pi/pll_controller_pi.c \
    ../../phase_lock/pll_voltage_oscillator/vco_controller.c \
    ../../notch_filter/notch_filter.c \
    $LIBS

if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

# Run the test (status messages go to console, data to file)
echo "Running PLL test..."
./pll_test

# Run the plotting script
echo "Generating plots..."
python3 plot_results.py
