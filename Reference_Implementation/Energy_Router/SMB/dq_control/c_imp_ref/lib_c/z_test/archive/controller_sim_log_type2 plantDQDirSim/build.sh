#!/bin/bash

echo "Current directory: $(pwd)"
echo "=== Compiling grid simulation ==="

gcc -o grid_simulation \
    main.c \
    grid_simulation.c \
    ./plant_simulator.c \
    ../../notch_filter/notch_filter.c \
    ../../log_data_rw/log_data_rw2.c \
    ../../dq_controller_pid/dq_controller_pid.c \
    ../../beta_transform/beta_transform_1p.c \
    ../../dq_to_modulation/dq_to_modulation.c \
    ../../dq_transform/dq_transform_1phase.c \
    -I../../ \
    -lm

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "=== Running simulation ==="
    ./grid_simulation
    
    echo "=== Creating plots ==="
    python3 plot_results.py
else
    echo "Build failed!"
fi
