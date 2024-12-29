import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Load data
try:
    data = pd.read_csv('simulation_results.csv')
except FileNotFoundError:
    print("Error: Could not find simulation_results.csv in current directory")
    exit(1)

t = data['t'].values
v_grid = data['v_grid'].values
i_load = data['i_load'].values
v_d_desired = data['v_d_desired'].values
v_q_desired = data['v_q_desired'].values
v_inv_alpha = data['v_inv_alpha'].values
v_inv_beta = data['v_inv_beta'].values
i_alpha = data['i_alpha'].values
i_beta = data['i_beta'].values
i_d = data['i_d'].values
i_q = data['i_q'].values
mod_index = data['mod_index'].values
phase_shift = data['phase_shift'].values

# Create figure with multiple subplots (adding 2 new ones)
fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1, figsize=(12, 12))

# Plot 1: Grid Voltage and Load Current overlaid (existing)
ax1_twin = ax1.twinx()
l1, = ax1.plot(t*1000, v_grid, 'b-', label='Grid Voltage')
l2, = ax1_twin.plot(t*1000, i_load, 'r-', label='Load Current')
l3, = ax1.plot(t*1000, v_inv_alpha, 'g-', label='Inverter Voltage Alpha')
ax1.set_xlabel('Time (ms)')
ax1.set_ylabel('Voltage (V)', color='b')
ax1_twin.set_ylabel('Current (A)', color='r')
ax1.tick_params(axis='y', labelcolor='b')
ax1_twin.tick_params(axis='y', labelcolor='r')
lines = [l1, l2, l3]
labels = [l.get_label() for l in lines]
ax1.legend(lines, labels, loc='upper right')
ax1.grid(True)

# Plot 2: Alpha-Beta Currents (new)
ax2.plot(t*1000, i_alpha, label='I_alpha')
ax2.plot(t*1000, i_beta, label='I_beta')
ax2.set_xlabel('Time (ms)')
ax2.set_ylabel('Current (A)')
ax2.set_title('Alpha-Beta Frame Currents')
ax2.grid(True)
ax2.legend()

# Plot 3: D-Q Currents (new)
ax3.plot(t*1000, i_d, label='I_d')
ax3.plot(t*1000, i_q, label='I_q')
ax3.set_xlabel('Time (ms)')
ax3.set_ylabel('Current (A)')
ax3.set_title('D-Q Frame Currents')
ax3.grid(True)
ax3.legend()

# Plot 4: Modulation (existing)
ax4.plot(t*1000, mod_index, label='Modulation Index')
ax4.plot(t*1000, phase_shift, label='Phase Shift')
ax4.set_xlabel('Time (ms)')
ax4.set_ylabel('Value')
ax4.legend()
ax4.grid(True)

plt.tight_layout()
plt.show()