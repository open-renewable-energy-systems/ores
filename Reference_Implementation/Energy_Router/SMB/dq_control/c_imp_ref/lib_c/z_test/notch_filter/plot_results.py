import numpy as np
import matplotlib.pyplot as plt

# Load the data
data = np.loadtxt('filter_results.txt', delimiter=',')
pure_signal = data[:, 0]
input_signal = data[:, 1]
output_signal = data[:, 2]

# Create time array
fs = 1000  # Sampling frequency in Hz
t = np.arange(len(input_signal)) / fs

# Control number of points to plot
plot_points = 300  # Adjust this value to show more or fewer points
end_idx = min(plot_points, len(input_signal))  # Use either plot_points or full length, whichever is smaller

# Create the plots (2 subplots)
plt.figure(figsize=(12, 10))

# Time domain plot
plt.subplot(2, 1, 1)
plt.plot(t[:end_idx], pure_signal[:end_idx], 'g-', label='Pure Signal', alpha=0.7)
plt.plot(t[:end_idx], input_signal[:end_idx], 'b-', label='Input Signal', alpha=0.7)
plt.plot(t[:end_idx], output_signal[:end_idx], 'r-', label='Filtered Signal', alpha=0.7)
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.title('Notch Filter Results - Time Domain')
plt.grid(True)
plt.legend()

# Frequency domain plot
plt.subplot(2, 1, 2)
freq_input = np.abs(np.fft.fft(input_signal))
freq_output = np.abs(np.fft.fft(output_signal))
freqs = np.fft.fftfreq(len(input_signal), 1/fs)

# Convert to dB
freq_input_db = 20 * np.log10(freq_input / np.max(freq_input))  # Normalize by max value
freq_output_db = 20 * np.log10(freq_output / np.max(freq_output))

# Only plot positive frequencies up to fs/2
positive_freq_mask = freqs >= 0
plt.plot(freqs[positive_freq_mask], freq_input_db[positive_freq_mask], 'b-', label='Input Spectrum', alpha=0.7)
plt.plot(freqs[positive_freq_mask], freq_output_db[positive_freq_mask], 'r-', label='Filtered Spectrum', alpha=0.7)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude (dB)')
plt.title('Frequency Response')
plt.grid(True)
plt.legend()

plt.tight_layout()
plt.show()