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

# Calculate time window
start_time = df['time_ms'].min() + 1000  # 1s after start
end_time = start_time + 200  # 200ms window
mask_200ms = (df['time_ms'] >= start_time) & (df['time_ms'] <= end_time)
df_200ms = df[mask_200ms].copy()

# Create three separate figures
# First figure (full data - computed only)
plt.figure(1, figsize=(15, 10))
gs1 = plt.GridSpec(2, 2)

# Time domain plots on the left
ax1 = plt.subplot(gs1[0, 0])
ax2 = plt.subplot(gs1[1, 0])
# Phase plot on the right
ax3 = plt.subplot(gs1[:, 1])

# Plot 1: Original current and beta (computed)
ax1.plot(df['time_ms'], df['curr_value'], label='Current (α)', color='blue')
ax1.plot(df['time_ms'], df['beta_offline'], label='Beta (β)', color='red')
ax1.set_ylabel('Amplitude')
ax1.set_title('Current and Beta Components (Computed)')
ax1.legend()
ax1.grid(True)

# Plot 2: D-Q components (computed)
ax2.plot(df['time_ms'], df['d_offline'], label='D', color='green', alpha=0.3)
ax2.plot(df['time_ms'], df['filtered_d_offline'], label='D (Filtered)', color='green', linewidth=2)
ax2.plot(df['time_ms'], df['notch_d'], label='D (Notched)', color='lightgreen', linewidth=2)
ax2.plot(df['time_ms'], df['q_offline'], label='Q', color='purple', alpha=0.3)
ax2.plot(df['time_ms'], df['filtered_q_offline'], label='Q (Filtered)', color='purple', linewidth=2)
ax2.plot(df['time_ms'], df['notch_q'], label='Q (Notched)', color='plum', linewidth=2)
ax2.set_xlabel('Time (ms)')
ax2.set_ylabel('Amplitude')
ax2.set_title('D-Q Components (Computed)')
ax2.legend()
ax2.grid(True)

# Plot 3: D-Q Phase Plot (computed)
ax3.plot(df['d_offline'], df['q_offline'], 'b.', label='Original D-Q', alpha=0.2, markersize=1)
ax3.plot(df['filtered_d_offline'], df['filtered_q_offline'], 'r.', label='Filtered D-Q', alpha=0.3, markersize=1)
ax3.set_aspect('equal')
ax3.grid(True)
ax3.set_xlabel('D Component')
ax3.set_ylabel('Q Component')
ax3.set_title('D-Q Phase Plot (Computed)')
ax3.legend()

plt.tight_layout()
plt.savefig('dq_analysis_results_full.png', dpi=300)
plt.show()

# Second figure: 200ms window (computed only)
plt.figure(2, figsize=(12, 8))

# First subplot: Current and Beta
ax1 = plt.subplot(2, 1, 1)
ax1.plot(df_200ms['time_ms'], df_200ms['curr_value'], label='Current (α)', color='blue')
ax1.plot(df_200ms['time_ms'], df_200ms['beta_offline'], label='Beta (β)', color='red')
ax1.set_ylabel('Amplitude')
ax1.set_title(f'Current and Beta Components (Computed) ({start_time:.0f}ms - {end_time:.0f}ms)')
ax1.legend()
ax1.grid(True)
ax1.set_xlim(start_time, end_time)

# Second subplot: D-Q components
ax2 = plt.subplot(2, 1, 2)
ax2.plot(df_200ms['time_ms'], df_200ms['d_offline'], label='D', color='green', alpha=0.3)
ax2.plot(df_200ms['time_ms'], df_200ms['filtered_d_offline'], label='D (Filtered)', color='green', linewidth=2)
ax2.plot(df_200ms['time_ms'], df_200ms['notch_d'], label='D (Notched)', color='green', linestyle='--', linewidth=2)
ax2.plot(df_200ms['time_ms'], df_200ms['q_offline'], label='Q', color='purple', alpha=0.3)
ax2.plot(df_200ms['time_ms'], df_200ms['filtered_q_offline'], label='Q (Filtered)', color='purple', linewidth=2)
ax2.plot(df_200ms['time_ms'], df_200ms['notch_q'], label='Q (Notched)', color='purple', linestyle='--', linewidth=2)
ax2.set_xlabel('Time (ms)')
ax2.set_ylabel('Amplitude')
ax2.set_title(f'D-Q Components (Computed) - {start_time:.0f}ms - {end_time:.0f}ms')
ax2.legend()
ax2.grid(True)
ax2.set_xlim(start_time, end_time)

plt.tight_layout()
plt.savefig('dq_analysis_results_200ms.png', dpi=300)
plt.show()

# Third figure: Comparison between computed and real-time signals (200ms window)
plt.figure(3, figsize=(15, 12))

# Beta comparison
plt.subplot(3, 1, 1)
plt.plot(df_200ms['time_ms'], df_200ms['beta_offline'], label='Beta (Computed)', color='blue')
plt.plot(df_200ms['time_ms'], df_200ms['beta_realtime'], label='Beta (Real-time)', color='red', linestyle='--')
plt.ylabel('Amplitude')
plt.title('Beta Comparison')
plt.legend()
plt.grid(True)
plt.xlim(start_time, end_time)

# D-Q comparison
plt.subplot(3, 1, 2)
plt.plot(df_200ms['time_ms'], df_200ms['d_offline'], label='D (Computed)', color='green')
plt.plot(df_200ms['time_ms'], df_200ms['d_realtime'], label='D (Real-time)', color='green', linestyle='--')
plt.plot(df_200ms['time_ms'], df_200ms['q_offline'], label='Q (Computed)', color='purple')
plt.plot(df_200ms['time_ms'], df_200ms['q_realtime'], label='Q (Real-time)', color='purple', linestyle='--')
plt.ylabel('Amplitude')
plt.title('D-Q Comparison')
plt.legend()
plt.grid(True)
plt.xlim(start_time, end_time)

# Filtered D-Q comparison
plt.subplot(3, 1, 3)
plt.plot(df_200ms['time_ms'], df_200ms['filtered_d_offline'], label='Filtered D (Computed)', color='green')
plt.plot(df_200ms['time_ms'], df_200ms['filtered_d_realtime'], label='Filtered D (Real-time)', color='green', linestyle='--')
plt.plot(df_200ms['time_ms'], df_200ms['filtered_q_offline'], label='Filtered Q (Computed)', color='purple')
plt.plot(df_200ms['time_ms'], df_200ms['filtered_q_realtime'], label='Filtered Q (Real-time)', color='purple', linestyle='--')
plt.xlabel('Time (ms)')
plt.ylabel('Amplitude')
plt.title('Filtered D-Q Comparison')
plt.legend()
plt.grid(True)
plt.xlim(start_time, end_time)

plt.tight_layout()
plt.savefig('comparison_analysis_200ms.png', dpi=300)
plt.show()

# Print comparison statistics
print("\nComparison Statistics:")
stats_df = pd.DataFrame({
    'Beta_diff': (df['beta_offline'] - df['beta_realtime']).describe(),
    'D_diff': (df['d_offline'] - df['d_realtime']).describe(),
    'Q_diff': (df['q_offline'] - df['q_realtime']).describe(),
    'Filtered_D_diff': (df['filtered_d_offline'] - df['filtered_d_realtime']).describe(),
    'Filtered_Q_diff': (df['filtered_q_offline'] - df['filtered_q_realtime']).describe()
})
print(stats_df)