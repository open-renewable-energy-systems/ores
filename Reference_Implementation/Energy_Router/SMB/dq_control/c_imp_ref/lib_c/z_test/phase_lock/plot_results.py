#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import os
import sys

# Define the absolute path to the results file
file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pll_test_results.txt")

# Add debug information
print(f"Current working directory: {os.getcwd()}")
print(f"Looking for file at: {file_path}")
print(f"File exists: {os.path.exists(file_path)}")

try:
    # Load from same directory as this Python file
    data = np.loadtxt("pll_test_results.txt", skiprows=1)
except FileNotFoundError:
    print("Error: Could not find pll_test_results.txt")
    print("Please ensure the C program has been run and generated the output file.")
    sys.exit(1)
except Exception as e:
    print(f"Error loading data: {str(e)}")
    sys.exit(1)

time = data[:, 0]
grid_v = data[:, 1]
pd_err = data[:, 2]
real_freq = data[:, 3]
est_freq = data[:, 4]
est_phase = data[:, 5]
true_phase_err = data[:, 6]
notch_out = data[:, 7]
lpf_out = data[:, 8]
control = data[:, 9]
frq_correction = data[:, 10]

# Create figure with subplots
fig, (ax1, ax2, ax3, ax4, ax5) = plt.subplots(5, 1, figsize=(12, 15))
# fig.suptitle('PLL Performance Analysis')

# Grid Voltage
ax1.plot(time, grid_v, label='Grid Voltage')
ax1.set_ylabel('Voltage (V)')
# ax1.set_title('Grid Voltage')
ax1.grid(True)
ax1.legend()

# Phase Detector Error and Filtered Error
ax2.plot(time, pd_err, label='PD Error', alpha=0.7)
ax2.plot(time, notch_out, label='PD after Notch Filter', alpha=0.7)
ax2.plot(time, lpf_out, label='PD after LPF', alpha=0.7)
ax2.set_ylabel('Error')
# ax2.set_title('Phase Detector Error and Filtered Error')
ax2.grid(True)
ax2.legend()

# VCO Correction Frequency
ax3.plot(time, control, label='PI Control Freq', alpha=0.7)
ax3.plot(time, frq_correction, label='VCO Correction Freq', alpha=0.7)
ax3.set_ylabel('Frequency (Hz)')
# ax3.set_title('Control Frq Vs. Correction Frq Comparison')
ax3.grid(True)
ax3.legend()

# Frequency Comparison
ax4.plot(time, est_freq, label='Estimated Frequency')
ax4.plot(time, real_freq, label='Grid Frequency')
ax4.set_ylabel('Frequency (Hz)')
# ax4.set_title('Frequency Comparison')
ax4.grid(True)
ax4.legend()

# Phase Error
ax5.plot(time, true_phase_err, label='True Phase Error')
ax5.set_ylabel('Phase Error (rad)')
# ax5.set_title('True Phase Error')
ax5.grid(True)
ax5.legend()

# Control Signal
# ax6.plot(time, control, label='PI Control Signal')
# ax6.set_xlabel('Time (s)')
# ax6.set_ylabel('Control Signal')
# # ax6.set_title('PI Controller Output')
# ax6.grid(True)
# ax6.legend()

# Adjust layout to prevent overlap
plt.tight_layout()
plt.show()