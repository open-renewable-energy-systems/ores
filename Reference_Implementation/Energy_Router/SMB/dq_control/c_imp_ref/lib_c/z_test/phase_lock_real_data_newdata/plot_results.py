#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the CSV file
data = pd.read_csv('pll_results.csv')

# Create subplots
fig, axs = plt.subplots(5, 1, figsize=(15, 12))
fig.suptitle('PLL Performance Analysis', fontsize=14)

# Plot 1: Input Signal and VCO Phase
axs[0].plot(data['time'], data['input_signal'], label='Input Signal', linewidth=2)
axs[0].plot(data['time'], np.sin(data['vco_phase']), label='PLL Output', linewidth=2, linestyle='--')
axs[0].set_ylabel('Amplitude')
axs[0].set_title('Signal Comparison')
axs[0].legend(loc='upper right')
axs[0].grid(True)

# Plot 2: Phase Comparison
axs[1].plot(data['time'], data['vco_phase'], label='PLL Phase', linewidth=2)
axs[1].plot(data['time'], data['real_angle'], label='Real Phase', linewidth=2, linestyle='--')
axs[1].set_ylabel('Phase (rad)')
axs[1].set_title('Phase Tracking')
axs[1].legend(loc='upper right')
axs[1].grid(True)

# Plot 3: Phase Error
axs[2].plot(data['time'], data['phase_diff'], label='Phase Error', linewidth=2, color='red')
axs[2].axhline(y=np.pi, color='k', linestyle='--', alpha=0.5, label='±π bound')
axs[2].axhline(y=-np.pi, color='k', linestyle='--', alpha=0.5)
axs[2].axhline(y=0, color='k', linestyle='-', alpha=0.3)
axs[2].set_ylabel('Error (rad)')
axs[2].set_title('Phase Error')
axs[2].set_ylim([-np.pi*1.2, np.pi*1.2])
axs[2].legend(loc='upper right')
axs[2].grid(True)

# Plot 4: PI Controller Output
axs[3].plot(data['time'], data['pi_output'], label='PI Output', linewidth=2, color='purple')
axs[3].set_ylabel('Control Signal')
axs[3].set_title('PI Controller Response')
axs[3].legend(loc='upper right')
axs[3].grid(True)

# Plot 5: Frequency
axs[4].plot(data['time'], data['vco_freq'], label='PLL Frequency', linewidth=2, color='green')
axs[4].axhline(y=50, color='r', linestyle='--', label='Nominal (50 Hz)')
axs[4].set_ylabel('Frequency (Hz)')
axs[4].set_xlabel('Time (s)')
axs[4].set_title('Frequency Tracking')
axs[4].legend(loc='upper right')
axs[4].grid(True)

# Improve layout
plt.tight_layout()
plt.subplots_adjust(top=0.92)

# Add statistics to phase error plot
phase_error_std = np.std(data['phase_diff'])
phase_error_mean = np.mean(data['phase_diff'])
stats_text = f'Error Statistics:\nMean: {phase_error_mean:.3f} rad\nStd: {phase_error_std:.3f} rad'
axs[2].text(0.02, 0.95, stats_text, transform=axs[2].transAxes, 
            bbox=dict(facecolor='white', alpha=0.8), fontsize=10)

# Save the plot
plt.savefig('pll_analysis.png', dpi=300, bbox_inches='tight')
plt.show()