import numpy as np
import matplotlib.pyplot as plt

# Constants matching the C code
FREQ_HZ = 50
SAMPLE_RATE_HZ = 1000
TEST_DURATION_SEC = 2.0  # Full test duration
PLOT_DURATION_SEC = 0.2  # Only plot last 200ms for time domain
NUM_SAMPLES = int(TEST_DURATION_SEC * SAMPLE_RATE_HZ)
PLOT_SAMPLES = int(PLOT_DURATION_SEC * SAMPLE_RATE_HZ)
SIGNAL_AMPLITUDE = 1.0

def plot_results(data, phase_offset):
    """Plot results for a single phase offset test"""
    # Calculate indices for the last 200ms
    plot_start_idx = -PLOT_SAMPLES  # Last 200ms
    
    # Extract the last 200ms of data
    t = data[plot_start_idx:, 0]      # Time column
    alpha = data[plot_start_idx:, 1]  # Alpha column
    beta = data[plot_start_idx:, 2]   # Beta column
    d = data[plot_start_idx:, 4]      # D column
    q = data[plot_start_idx:, 5]      # Q column
    
    # Calculate means from the plotted data
    d_mean = np.mean(d)
    q_mean = np.mean(q)
    
    # Create figure with 2 subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
    fig.suptitle(f'DQ Transform Results (Phase Offset: {phase_offset:.2f} rad)\nLast 200ms of 2s simulation')
    
    # Adjust time to start from 1800ms
    t_ms = t * 1000  # Convert to ms
    t_offset = t_ms - t_ms[0]  # Make time start from 0
    t_display = t_offset + (TEST_DURATION_SEC - PLOT_DURATION_SEC) * 1000  # Shift to show actual time
    
    # Plot input signals
    ax1.plot(t_display, alpha, 'b-', label='Alpha')
    ax1.plot(t_display, beta, 'r-', label='Beta')
    ax1.set_xlabel('Time (ms)')
    ax1.set_ylabel('Amplitude')
    ax1.set_title('Input Signals')
    ax1.grid(True)
    ax1.legend()
    
    # Plot DQ outputs
    ax2.plot(t_display, d, 'g-', label=f'D (mean={d_mean:.3f})')
    ax2.plot(t_display, q, 'm-', label=f'Q (mean={q_mean:.3f})')
    ax2.set_xlabel('Time (ms)')
    ax2.set_ylabel('Amplitude')
    ax2.set_title('DQ Components')
    ax2.grid(True)
    ax2.legend()
    
    # Set x-axis limits to show 1800-2000ms
    ax1.set_xlim(1800, 2000)
    ax2.set_xlim(1800, 2000)
    
    plt.tight_layout()
    plt.savefig(f'build/dq_results_phase_{phase_offset:.2f}.png')
    plt.show()
    plt.close()
    
    return d_mean, q_mean

def get_expected_dq(phase_offset):
    """Calculate expected D-Q values for a given phase offset"""
    d_expected = SIGNAL_AMPLITUDE * np.cos(phase_offset)
    q_expected = SIGNAL_AMPLITUDE * np.sin(phase_offset)
    return d_expected, q_expected

def main():
    # Phase offsets to test
    phase_offsets = [0.0, np.pi/2, np.pi]
    
    # Arrays to store results
    d_actuals = []
    q_actuals = []
    d_expecteds = []
    q_expecteds = []
    
    for phase_offset in phase_offsets:
        filename = f'build/dq_results_phase_{phase_offset:.2f}.csv'
        try:
            data = np.loadtxt(filename, delimiter=',', skiprows=1)
            d_mean, q_mean = plot_results(data, phase_offset)
            
            # Get expected values
            d_expected, q_expected = get_expected_dq(phase_offset)
            
            # Store results for comparison
            d_actuals.append(d_mean)
            q_actuals.append(q_mean)
            d_expecteds.append(d_expected)
            q_expecteds.append(q_expected)
            
            print(f"\nResults for phase offset {phase_offset:.2f} rad:")
            print(f"D component: actual={d_mean:.3f}, expected={d_expected:.3f}, "
                  f"error={100*(d_mean-d_expected)/d_expected:.1f}%")
            print(f"Q component: actual={q_mean:.3f}, expected={q_expected:.3f}, "
                  f"error={100*(q_mean-q_expected)/q_expected:.1f}% if q_expected != 0 else 'N/A'")
            
        except FileNotFoundError:
            print(f"Error: Could not find {filename}")
            continue
    
    # Create comparison plot
    if d_actuals:
        plt.figure(figsize=(10, 6))
        plt.plot(phase_offsets, d_actuals, 'bo-', label='D Actual')
        plt.plot(phase_offsets, d_expecteds, 'b--', label='D Expected')
        plt.plot(phase_offsets, q_actuals, 'ro-', label='Q Actual')
        plt.plot(phase_offsets, q_expecteds, 'r--', label='Q Expected')
        plt.xlabel('Phase Offset (rad)')
        plt.ylabel('Amplitude')
        plt.title('DQ Components vs Phase Offset\n(Averaged over last 200ms)')
        plt.grid(True)
        plt.legend()
        plt.savefig('build/dq_transform_comparison.png')
        plt.show()
        plt.close()

if __name__ == "__main__":
    main()