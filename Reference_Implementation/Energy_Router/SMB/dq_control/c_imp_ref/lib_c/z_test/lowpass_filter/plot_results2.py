import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq

# Read data from file
data = np.genfromtxt('lpf_results.txt', delimiter=',', skip_header=1)
time = data[:, 0]
input_signal = data[:, 1]
single_filter = data[:, 2]
double_filter = data[:, 3]
phase90 = data[:, 4]

# Create time domain plot
plt.figure(figsize=(12, 8))

# Plot all signals
# plt.plot(time, input_signal, label='Input Signal', alpha=0.7)
# plt.plot(time, single_filter, label='Single Filter', alpha=0.7)
plt.plot(time, double_filter, label='Double Filter', alpha=0.7)
plt.plot(time, phase90, label='True 90° Phase', linestyle='--', alpha=0.7)

plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.title('Phase Delay Comparison')
plt.grid(True)
plt.legend()

# Add vertical lines at peaks for phase comparison
peak_time = 0.005  # Time of first peak for input signal
plt.axvline(x=peak_time, color='gray', linestyle=':', alpha=0.5)
plt.axvline(x=peak_time + 0.005, color='gray', linestyle=':', alpha=0.5)  # 90° phase at 50Hz = 5ms

plt.tight_layout()
plt.savefig('phase_delay_results.png')
plt.show()