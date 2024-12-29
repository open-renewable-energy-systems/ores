#!/bin/bash

echo "Building PWM control test..."

# Compile the code
gcc -o stair_wave_test main.c \
    ../../pwm/stair_wave/four_modules/lookup_table_1d/dyn_stair_wave_table.c \
    ../../pwm/stair_wave/four_modules/load_table_5d/load_switching_angles_table_5d.c \
    ../../pwm/stair_wave/four_modules/interp_table_5d/interp_table_5d.c \
    ../../pwm/stair_wave/four_modules/stair_wave_pwm/stair_wave_pwm.c \
    ../../pwm/stair_wave/common/write_ios/write_ios.c \
    ../../pwm/stair_wave/common/modulation_2pwm/modulation_2pwm.c \
    -I../../pwm/stair_wave/four_modules/lookup_table_1d \
    -I../../pwm/stair_wave/four_modules/load_table_5d \
    -I../../pwm/stair_wave/four_modules/interp_table_5d \
    -I../../pwm/stair_wave/four_modules/stair_wave_pwm \
    -I../../pwm/stair_wave/common/write_ios \
    -I../../pwm/stair_wave/common/modulation_2pwm \
    -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Running test..."
    ./stair_wave_test
    
    # Check if the test program ran successfully
    if [ $? -eq 0 ]; then
        echo "Test completed successfully!"
        
        # Run the Python plotting script
        echo "Generating plot..."
        # python3 plot_results.py
    else
        echo "Test failed!"
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi