import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq

# Read data from file
data = np.genfromtxt('lpf_results.txt', delimiter=',', skip_header=1)
time = data[:, 0]
input_signal = data[:, 1]
output_signal = data[:, 2]
original_signal = data[:, 3]

# Create time domain plot
plt.figure(figsize=(12, 8))
plt.subplot(2, 1, 1)
plt.plot(time, input_signal, label='Input (with harmonics)', alpha=0.7)
plt.plot(time, output_signal, label='Filtered Output', alpha=0.7)
plt.plot(time, original_signal, label='Original (50Hz)', alpha=0.7, linestyle='--')
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.title('Time Domain Response')
plt.grid(True)
plt.legend()

# Create frequency domain plot
N = len(time)
T = 1.0 / 1000.0  # sampling period
freq = fftfreq(N, T)[:N//2]

input_fft = np.abs(fft(input_signal))[:N//2]
output_fft = np.abs(fft(output_signal))[:N//2]
original_fft = np.abs(fft(original_signal))[:N//2]

plt.subplot(2, 1, 2)
plt.plot(freq, input_fft, label='Input (with harmonics)', alpha=0.7)
plt.plot(freq, output_fft, label='Filtered Output', alpha=0.7)
plt.plot(freq, original_fft, label='Original (50Hz)', alpha=0.7, linestyle='--')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.title('Frequency Domain Response')
plt.grid(True)
plt.legend()
plt.xlim(0, 300)  # Show up to 300 Hz

plt.tight_layout()
plt.savefig('lpf_results.png')
plt.show()