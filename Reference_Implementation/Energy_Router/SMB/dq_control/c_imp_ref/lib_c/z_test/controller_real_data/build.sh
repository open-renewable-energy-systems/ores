#!/bin/sh

set -e  # Exit immediately if a command exits with a non-zero status

echo "Current directory: $(pwd)"
echo "=== Compiling grid simulation ==="

# Compile all source files
gcc -c -Wall -I. -I../../ -D_USE_MATH_DEFINES \
    ../../log_data_rw/log_data_rw.c \
    ../../dq_controller_pid/dq_controller_pid.c \
    ../../dq_to_modulation/dq_to_modulation.c \
    ../../beta_transform/beta_transform_1p.c \
    ../../dq_transform/dq_transform_1phase.c \
    ../../misc/power_2dq_ref/power_2dq_ref.c \
    grid_simulation.c \
    plant_simulator.c \
    main.c

# Link them together
gcc *.o -o controller_real_data -lm

echo "Build successful!"
./controller_real_data

# Run the Python plotting script
python3 plot_results.py

# Clean up object files
rm -f *.o