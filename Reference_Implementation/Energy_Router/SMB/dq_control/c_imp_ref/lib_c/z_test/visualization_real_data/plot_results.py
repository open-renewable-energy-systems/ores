import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the CSV file
df = pd.read_csv('comparison_data.csv')

# Print available columns for debugging
print("Available columns in CSV:")
print(df.columns.tolist())

# Convert time_us to time_ms
df['time_ms'] = df['time_us'] / 1000.0

# Create single figure with all plots
plt.figure(1, figsize=(15, 20))
gs = plt.GridSpec(4, 1)

# Original plots (current/beta and D-Q components)
ax1 = plt.subplot(gs[0])
ax1.plot(df['time_ms'], df['curr_value'], label='Current (α)', color='blue')
ax1.plot(df['time_ms'], df['beta_offline'], label='Beta (β)', color='red')
ax1.set_ylabel('Amplitude')
ax1.set_title('Current and Beta Components (Computed)')
ax1.legend()
ax1.grid(True)

ax2 = plt.subplot(gs[1])
ax2.plot(df['time_ms'], df['d_offline'], label='D', color='green', alpha=0.3)
ax2.plot(df['time_ms'], df['filtered_d_offline'], label='D (Filtered)', color='green', linewidth=2)
ax2.plot(df['time_ms'], df['notch_d'], label='D (Notched)', color='lightgreen', linewidth=2)
ax2.plot(df['time_ms'], df['q_offline'], label='Q', color='purple', alpha=0.3)
ax2.plot(df['time_ms'], df['filtered_q_offline'], label='Q (Filtered)', color='purple', linewidth=2)
ax2.plot(df['time_ms'], df['notch_q'], label='Q (Notched)', color='plum', linewidth=2)
ax2.set_ylabel('Amplitude')
ax2.set_title('D-Q Components (Computed)')
ax2.legend()
ax2.grid(True)

# D-Q comparison plot
ax3 = plt.subplot(gs[2])
ax3.plot(df['time_ms'], df['d_offline'], label='D (Computed)', color='green')
ax3.plot(df['time_ms'], df['d_realtime'], label='D (Real-time)', color='lightgreen', linestyle='--')
ax3.plot(df['time_ms'], df['q_offline'], label='Q (Computed)', color='purple')
ax3.plot(df['time_ms'], df['q_realtime'], label='Q (Real-time)', color='plum', linestyle='--')
ax3.set_ylabel('Amplitude')
ax3.set_title('D-Q Components Comparison')
ax3.legend()
ax3.grid(True)

# Control parameters plot
ax4 = plt.subplot(gs[3])
ax4_twin = ax4.twinx()  # Create twin axis for second y-axis

# Plot on primary y-axis (Phase Shift)
line1 = ax4.plot(df['time_ms'], df['phase_shift'], label='Phase Shift', color='blue')
ax4.set_ylabel('Phase Shift (rad)', color='blue')
ax4.tick_params(axis='y', labelcolor='blue')

# Plot on secondary y-axis (Modulation Index)
line2 = ax4_twin.plot(df['time_ms'], df['mod_index'], label='Modulation Index', color='red')
ax4_twin.set_ylabel('Modulation Index', color='red')
ax4_twin.tick_params(axis='y', labelcolor='red')

ax4.set_xlabel('Time (ms)')
ax4.set_title('Control Parameters')
ax4.grid(True)

# Combine legends from both y-axes
lines = line1 + line2
labels = [l.get_label() for l in lines]
ax4.legend(lines, labels)

plt.tight_layout()
plt.savefig('all_analysis.png', dpi=300)
plt.show()