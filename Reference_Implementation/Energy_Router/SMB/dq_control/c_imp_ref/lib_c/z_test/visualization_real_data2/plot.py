import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
df = pd.read_csv('Log_充电_6A.csv')

# Convert time from microseconds to milliseconds
time_ms = (df['time_us'] - df['time_us'].iloc[0]) / 1000

# Create a figure with multiple subplots
plt.style.use('default')
fig, axs = plt.subplots(5, 1, figsize=(15, 25))  # Changed to 5 subplots
fig.suptitle('Power System Parameters Visualization', fontsize=16)

# Plot 1: Alpha-Beta Currents
axs[0].plot(time_ms, df['i_alpha'], label='i_alpha')
axs[0].plot(time_ms, df['i_beta'], label='i_beta')
axs[0].set_title('Alpha-Beta Currents')
axs[0].set_ylabel('Current (A)')
axs[0].set_xlabel('Time (ms)')
axs[0].legend()
axs[0].grid(True)

# Plot 2: DQ Currents
axs[1].plot(time_ms, df['i_raw_d'], label='i_raw_d')
axs[1].plot(time_ms, df['i_raw_q'], label='i_raw_q')
axs[1].plot(time_ms, df['i_filtered_d'], label='i_filtered_d')
axs[1].plot(time_ms, df['i_filtered_q'], label='i_filtered_q')
axs[1].set_title('DQ Current Measurements')
axs[1].set_ylabel('Current (A)')
axs[1].set_xlabel('Time (ms)')
axs[1].legend()
axs[1].grid(True)

# Plot 3: Grid Voltages
axs[2].plot(time_ms, df['v_grid_meas'], label='v_grid_meas')
axs[2].plot(time_ms, df['v_grid_d'], label='v_grid_d')
axs[2].plot(time_ms, df['v_grid_q'], label='v_grid_q')
axs[2].set_title('Grid Voltages')
axs[2].set_ylabel('Voltage (V)')
axs[2].set_xlabel('Time (ms)')
axs[2].legend()
axs[2].grid(True)

# Plot 4: SMB Voltages
axs[3].plot(time_ms, df['v_smb_meas'], label='v_smb_meas')
axs[3].plot(time_ms, df['v_smb_d'], label='v_smb_d')
axs[3].plot(time_ms, df['v_smb_q'], label='v_smb_q')
axs[3].set_title('SMB Voltages')
axs[3].set_ylabel('Voltage (V)')
axs[3].set_xlabel('Time (ms)')
axs[3].legend()
axs[3].grid(True)

# Plot 5: Control Voltages
axs[4].plot(time_ms, df['v_cntl_d'], label='v_cntl_d')
axs[4].plot(time_ms, df['v_cntl_q'], label='v_cntl_q')
axs[4].plot(time_ms, df['v_dc'], label='v_dc')
axs[4].set_title('Control Voltages')
axs[4].set_ylabel('Voltage (V)')
axs[4].set_xlabel('Time (ms)')
axs[4].legend()
axs[4].grid(True)

# Adjust layout and display
plt.tight_layout()
plt.show()

# Optional: Save the figure
# plt.savefig('power_system_visualization.png', dpi=300, bbox_inches='tight')