import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Clear everything
plt.close('all')
plt.clf()

# Load data
data = pd.read_csv('simulation_results.csv')

# Print debug info for v_inv_alpha
print("v_inv_alpha statistics:")
print(f"Mean: {data['v_inv_alpha'].mean():.2f}")
print(f"Min: {data['v_inv_alpha'].min():.2f}")
print(f"Max: {data['v_inv_alpha'].max():.2f}")
print("\nFirst few v_inv_alpha values:")
print(data['v_inv_alpha'].head())
print("\nColumns in data:")
print(data.columns.tolist())

# Skip initial transient (first 10%) and end (last 10%) of the data
start_idx = int(len(data) * 0.1)
end_idx = int(len(data) * 0.9)
steady_state_data = data.iloc[start_idx:end_idx]

# Create figure with subplots (only one figure now)
fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1, figsize=(12, 12))

# Plot 1: Grid Voltage and Load Current overlaid with two y-axes
ax1_twin = ax1.twinx()
l1, = ax1.plot(steady_state_data['t']*1000, steady_state_data['v_grid'], 'b-', label='Grid Voltage')
l2, = ax1_twin.plot(steady_state_data['t']*1000, steady_state_data['i_load'], 'r-', label='Load Current')
l3, = ax1.plot(steady_state_data['t']*1000, steady_state_data['v_inv_alpha'], 'g-', label='Inverter Voltage Alpha')
ax1.set_xlabel('Time (ms)')
ax1.set_ylabel('Voltage (V)', color='b')
ax1_twin.set_ylabel('Current (A)', color='r')
ax1.tick_params(axis='y', labelcolor='b')
ax1_twin.tick_params(axis='y', labelcolor='r')
lines = [l1, l2, l3]
labels = [l.get_label() for l in lines]
ax1.legend(lines, labels, loc='upper right')
ax1.grid(True)

# Plot 2: Alpha-Beta Currents
ax2.plot(steady_state_data['t']*1000, steady_state_data['i_alpha'], 'b-', label='I_alpha')
ax2.plot(steady_state_data['t']*1000, steady_state_data['i_beta'], 'r-', label='I_beta')
ax2.set_xlabel('Time (ms)')
ax2.set_ylabel('Current (A)')
ax2.set_title('Alpha-Beta Frame Currents')
ax2.grid(True)
ax2.legend()

# Plot 3: D-Q Currents
ax3.plot(steady_state_data['t']*1000, steady_state_data['i_d'], 'b-', label='I_d')
ax3.plot(steady_state_data['t']*1000, steady_state_data['i_q'], 'r-', label='I_q')
ax3.set_xlabel('Time (ms)')
ax3.set_ylabel('Current (A)')
ax3.set_title('D-Q Frame Currents')
ax3.grid(True)
ax3.legend()

# Plot 4: Modulation and Phase Shifts
ax4.plot(steady_state_data['t']*1000, steady_state_data['mod_index'], 'b-', label='Modulation Index')
ax4.plot(steady_state_data['t']*1000, steady_state_data['phase_shift'], 'r-', label='Voltage Phase Shift')
ax4.plot(steady_state_data['t']*1000, steady_state_data['current_phase_shift'], 'g-', label='Current Phase Shift')
ax4.set_xlabel('Time (ms)')
ax4.set_ylabel('Value')
ax4.legend()
ax4.grid(True)

plt.tight_layout()
plt.show()