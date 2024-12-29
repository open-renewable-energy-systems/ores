import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq

# Read data from CSV
data = np.genfromtxt('beta_transform_data.csv', delimiter=',', skip_header=1)
time = data[:, 0]
alpha_clean = data[:, 1]  # Original pure sine signal
beta = data[:, 3]

# Define sampling frequency
fs = 1000  # sampling frequency (Hz)

# Generate ideal -cos reference
neg_cos = -np.cos(2 * np.pi * 50 * time)

# Get last 200ms of data
sample_period = 1/fs  # 1ms
samples_200ms = int(0.2 / sample_period)  # number of samples in 200ms
time_window = time[-samples_200ms:]
beta_window = beta[-samples_200ms:]
neg_cos_window = neg_cos[-samples_200ms:]
alpha_clean_window = alpha_clean[-samples_200ms:]  # Original signal window

# Create figure for time domain analysis
plt.figure(figsize=(15, 5))
plt.plot(time_window, beta_window, 'g-', label='Beta Output', linewidth=2)
plt.plot(time_window, neg_cos_window, 'k--', label='-cos (Ideal)', linewidth=2)
plt.plot(time_window, alpha_clean_window, 'b-', label='Original sin', linewidth=2)
plt.plot(time_window, np.zeros_like(time_window), 'r:', label='Noise Level (0)', linewidth=1)
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.title('Time Domain Comparison (Last 200ms)')
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig('beta_transform_time_analysis.png', dpi=300, bbox_inches='tight')
plt.show()

# FFT calculations
n = len(time)
freq = fftfreq(n, 1/fs)[:n//2]  # positive frequencies only

# Compute FFTs
beta_fft = fft(beta_steady)[:n//2]
neg_cos_fft = fft(neg_cos_steady)[:n//2]

# Create figure for frequency domain analysis
plt.figure(figsize=(15, 10))

# Magnitude plot
plt.subplot(2, 1, 1)
magnitude_scale = 2.0/n
magnitude_scale_array = np.ones(len(freq)) * magnitude_scale
magnitude_scale_array[0] = 1.0/n

plt.plot(freq, np.abs(beta_fft) * magnitude_scale_array, 'g-', 
         label='Beta Output', linewidth=2)
plt.plot(freq, np.abs(neg_cos_fft) * magnitude_scale_array, 'k--', 
         label='-cos (Ideal)', linewidth=2)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.title('Frequency Domain Analysis (Magnitude)')
plt.grid(True)
plt.legend()
plt.xlim(0, 200)

# Phase plot
plt.subplot(2, 1, 2)
plt.plot(freq, np.angle(beta_fft, deg=True), 'g-', 
         label='Beta Output', linewidth=2)
plt.plot(freq, np.angle(neg_cos_fft, deg=True), 'b--', 
         label='-cos (Ideal)', linewidth=2)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Phase (degrees)')
plt.title('Frequency Domain Analysis (Phase)')
plt.grid(True)
plt.legend()
plt.xlim(0, 200)
plt.ylim(-180, 180)

plt.tight_layout()
plt.savefig('beta_transform_freq_analysis.png', dpi=300, bbox_inches='tight')
plt.show()

# Print analysis at key frequencies
def analyze_at_freq(target_freq):
    freq_idx = np.argmin(np.abs(freq - target_freq))
    print(f"\nAnalysis at {target_freq}Hz:")
    print(f"Frequency bin: {freq[freq_idx]:.1f} Hz")
    print(f"Beta magnitude: {np.abs(beta_fft[freq_idx]) * 2/n:.3f}")
    print(f"-cos magnitude: {np.abs(neg_cos_fft[freq_idx]) * 2/n:.3f}")
    print(f"Beta phase: {np.angle(beta_fft[freq_idx], deg=True):.1f}°")
    print(f"-cos phase: {np.angle(neg_cos_fft[freq_idx], deg=True):.1f}°")
    phase_diff = (np.angle(beta_fft[freq_idx], deg=True) - 
                 np.angle(neg_cos_fft[freq_idx], deg=True)) % 360
    print(f"Phase difference: {phase_diff:.1f}°")

# Analyze at fundamental and noise frequencies
analyze_at_freq(50)   # Fundamental
analyze_at_freq(150)  # Noise frequency