#!/bin/bash

# Get current directory
current_dir=$(pwd)
echo "Current directory: $current_dir"

# Compile the grid simulation
echo "=== Compiling grid simulation ==="
gcc -o grid_simulation \
    main.c \
    grid_simulation.c \
    plant_simulator.c \
    ../../beta_transform/beta_transform_1p.c \
    ../../dq_transform/dq_transform_1phase.c \
    ../../dq_controller_pid/dq_controller_pid.c \
    ../../dq_to_modulation/dq_to_modulation.c \
    -I../../ \
    -I../../lowpass_filter \
    -I. \
    -lm

if [ $? -eq 0 ]; then
    echo "Build successful!"
    # Run simulation
    echo "=== Running grid simulation ==="
    ./grid_simulation
    
    # Create plots
    echo "=== Creating plots ==="
    /usr/bin/python3 plot_results.py
else
    echo "Build failed!"
    exit 1
fi
# Rest of the script remains the same
